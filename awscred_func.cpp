/*
 * s3fs-fuse-awscred-lib ( s3fs-fuse credential I/F library for AWS )
 *
 *     Copyright 2022 Takeshi Nakatani <ggtakec@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string.h>
#include <iostream>
#include <algorithm>
#include <string>

#include "config.h"
#include "awscred.h"
#include "awscred_func.h"

//----------------------------------------------------------
// S3fsAwsCredParseOption
//----------------------------------------------------------
static size_t S3fsAwsCredParseOption(const char* options, std::map<std::string, std::string>& opts)
{
	opts.clear();
	if(nullptr == options || '\0' == options[0]){
		return opts.size();
	}
	std::string	strOptions = options;

	// Cut double/single quote pair
	if(2 <= strOptions.length() && (('"' == *strOptions.begin() && '"' == *strOptions.rbegin()) || ('\'' == *strOptions.begin() && '\'' == *strOptions.rbegin()))){
		strOptions = strOptions.substr(1, strOptions.length() - 2);
	}

	// Parse ',' and Set option/value
	std::string::size_type	FoundPos = std::string::npos;
	do{
		std::string::size_type	StartPos = (FoundPos == std::string::npos ? 0 : (FoundPos + 1));
		FoundPos = strOptions.find(',', StartPos);

		std::string	FoundPair;
		if(FoundPos == std::string::npos){
			FoundPair = strOptions;
		}else if(StartPos != FoundPos){
			FoundPair  = strOptions.substr(0, FoundPos);
			strOptions = strOptions.substr(FoundPos + 1);
		}

		if(!FoundPair.empty()){
			std::string				strLowKey;
			std::string				strValue;
			std::string::size_type	PairEqPos = FoundPair.find('=');
			if(PairEqPos == std::string::npos){
				strLowKey = FoundPair;
			}else{
				strLowKey = FoundPair.substr(0, PairEqPos);
				strValue  = FoundPair.substr(++PairEqPos);
			}
			std::transform(strLowKey.cbegin(), strLowKey.cend(), strLowKey.begin(), ::tolower);
			opts[strLowKey] = strValue;
		}
	}while(FoundPos != std::string::npos);

	return opts.size();
}

//----------------------------------------------------------
// Aws::SDKOptions
//----------------------------------------------------------
//
// Holds only one Aws::SDKOptions object
//
// [NOTE]
// This library expects to call the Aws::InitAPI() and Aws::ShutdownAPI() only once.
//
static Aws::SDKOptions& GetSDKOptions()
{
	static Aws::SDKOptions	options;
	return options;
}

//----------------------------------------------------------
// SSO Profile name
//----------------------------------------------------------
static Aws::String& GetSSOProfile()
{
	static Aws::String	ssoprofile;
	return ssoprofile;
}

//----------------------------------------------------------
// Auxiliary Valid period seconds
//----------------------------------------------------------
// [NOTE] About Session Token Expiration
// There is no key in .aws/config or .aws/credential that
// specifies the SessionToken Expiration.(missing keys like
// aws_session_expiration)
// This library only provides a function to load Credentials
// and does not have a function to obtain SessionTokens, so
// SessionTokens(and AccessKeys, Secrets, etc.) can only be
// obtained from Credentials (environment variables, files,
// etc).
// This means that unless you pass the Expriation externally,
// you won't know its expiration date.
// Therefore, it can only be passed as a constant value as
// an option to this library.(This may change in the future)
//
static int64_t	periodsec = -1;

static bool SetValidPeriodSec(int64_t sec)
{
	if(-1 != periodsec){
		return false;
	}
	if(sec <= 0 || (60 * 60 * 24 * 365 * 5) < sec){		// Maximum is 5 years
		return false;
	}
	periodsec = sec;

	return true;
}

static const Aws::Utils::DateTime& GetExparationByValidPeriod(const Aws::String& sessionToken, const Aws::Utils::DateTime& exp)
{
	static Aws::String			targetSessionToken;
	static Aws::Utils::DateTime	targetExpiration;

	if(-1 == periodsec){
		return exp;
	}
	if(targetSessionToken != sessionToken){
		// Update new session token
		int64_t	expms		= exp.Millis();
		int64_t	maxms		= Aws::Utils::DateTime::CurrentTimeMillis() + (periodsec * 1000);
		targetExpiration	= Aws::Utils::DateTime(std::min(expms, maxms));
		targetSessionToken	= sessionToken;
	}
	return targetExpiration;
}

//----------------------------------------------------------
// Export interface functions
//----------------------------------------------------------
//
// VersionS3fsCredential()
//
const char* VersionS3fsCredential(bool detail)
{
	const char short_version_form[]  = "s3fs-fuse-awscred-lib : Version %s (%s)";
	const char detail_version_form[] = 
		"s3fs-fuse-awscred-lib : Version %s (%s)\n"
		"s3fs-fuse credential I/F library for AWS\n"
		"Copyright 2022 Takeshi Nakatani <ggtakec@gmail.com>\n";

	static char short_version_string[128];
	static char detail_version_string[256];
	static bool is_init = false;

	if(!is_init){
		is_init = true;
		sprintf(short_version_string, short_version_form, product_version, commit_hash_version);
		sprintf(detail_version_string, detail_version_form, product_version, commit_hash_version);
	}
	if(detail){
		return detail_version_string;
	}else{
		return short_version_string;
	}
}

//
// InitS3fsCredential()
//
bool InitS3fsCredential(const char* popts, char** pperrstr)
{
	if(pperrstr){
		*pperrstr = NULL;
	}

	//
	// Check option arguments and set it
	//
	Aws::SDKOptions& options = GetSDKOptions();

	// Parse option string
	std::map<std::string, std::string>	ParsedOpts;
	size_t	OptCnt = S3fsAwsCredParseOption(popts, ParsedOpts);

	if(0 < OptCnt){
		bool	isSetLogLevel	= false;

		for(std::map<std::string, std::string>::const_iterator iter = ParsedOpts.begin(); iter != ParsedOpts.end(); ++iter){
			std::string	strLowkey	= iter->first;
			std::string	strValue	= iter->second;

			if(0 == strcasecmp(strLowkey.c_str(), "SSOProfile") || 0 == strcasecmp(strLowkey.c_str(), "SSOProf")){
				if(strValue.empty()){
					if(pperrstr){
						*pperrstr = strdup("Option(SSOProfile) value is empty.");
					}
					return false;
				}

				Aws::String&	ssoprofile = GetSSOProfile();
				if(!ssoprofile.empty()){
					if(pperrstr){
						*pperrstr = strdup("Already specified SSO Profile name.");
					}
					return false;
				}
				ssoprofile = strValue.c_str();

			}else if(0 == strcasecmp(strLowkey.c_str(), "TokenPeriodSecond") || 0 == strcasecmp(strLowkey.c_str(), "PeriodSec")){
				if(strValue.empty()){
					if(pperrstr){
						*pperrstr = strdup("Option(SSOProfile) value is empty.");
					}
					return false;
				}
				int64_t	periodsec = static_cast<int64_t>(stoll(strValue));

				if(!SetValidPeriodSec(periodsec)){
					if(pperrstr){
						*pperrstr = strdup("Failed to set Session Token Period Seconds.");
					}
					return false;
				}

			}else if(0 == strcasecmp(strLowkey.c_str(), "LogLevel")){
				if(0 == strcasecmp(strValue.c_str(), "Off")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Off;

				}else if(0 == strcasecmp(strValue.c_str(), "Fatal")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Fatal;

				}else if(0 == strcasecmp(strValue.c_str(), "Error")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Error;

				}else if(0 == strcasecmp(strValue.c_str(), "Warn")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Warn;

				}else if(0 == strcasecmp(strValue.c_str(), "Info")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;

				}else if(0 == strcasecmp(strValue.c_str(), "Debug")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;

				}else if(0 == strcasecmp(strValue.c_str(), "Trace")){
					if(isSetLogLevel){
						if(pperrstr){
							*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
						}
						return false;
					}
					options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
				}else{
					if(pperrstr){
						*pperrstr = strdup("Unknown option(LogLevel) value is specified.");
					}
					return false;
				}

			// [NOTE]
			// The key name only(no value) option is an abbreviation for the LogLevel option.
			//
			}else if(0 == strcasecmp(strLowkey.c_str(), "Off")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Off;

			}else if(0 == strcasecmp(strLowkey.c_str(), "Fatal")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Fatal;

			}else if(0 == strcasecmp(strLowkey.c_str(), "Error")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Error;

			}else if(0 == strcasecmp(strLowkey.c_str(), "Warn")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Warn;

			}else if(0 == strcasecmp(strLowkey.c_str(), "Info")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;

			}else if(0 == strcasecmp(strLowkey.c_str(), "Debug")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;

			}else if(0 == strcasecmp(strLowkey.c_str(), "Trace")){
				if(isSetLogLevel){
					if(pperrstr){
						*pperrstr = strdup("Option(LogLevel) is already specified, so could not set Off.");
					}
					return false;
				}
				options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
			}else{
				if(pperrstr){
					*pperrstr = strdup("Unknown option is specified.");
				}
				return false;
			}
		}
	}

	//
	// Initalize
	//
	Aws::InitAPI(options);

	return true;
}

//
// FreeS3fsCredential()
//
bool FreeS3fsCredential(char** pperrstr)
{
	if(pperrstr){
		*pperrstr = NULL;
	}

	//
	// Shotdown
	//
	Aws::ShutdownAPI(GetSDKOptions());

	return true;
}

//
// UpdateS3fsCredential()
//
bool UpdateS3fsCredential(char** ppaccess_key_id, char** ppserect_access_key, char** ppaccess_token, long long* ptoken_expire, char** pperrstr)
{
	if(!ppaccess_key_id || !ppserect_access_key || !ppaccess_token || !ptoken_expire){
		if(pperrstr){
			*pperrstr = strdup("Some parameters are wrong(NULL).");
		}
		return false;
	}
	if(pperrstr){
		*pperrstr = NULL;
	}

	// Get SSO Profile option
	const Aws::String&		ssoprofile	= GetSSOProfile();
	const char*				pSSOProf	= ssoprofile.empty() ? nullptr : ssoprofile.c_str();

	// Create provider chain
	S3fsAWSCredentialsProviderChain	providerChains(pSSOProf);
	Aws::SDKOptions&		options		= GetSDKOptions();
	auto					credentials	= providerChains.GetAWSCredentials();
	bool					result		= true;

	// Get credentials
	Aws::String				accessKeyId	= credentials.GetAWSAccessKeyId();
	Aws::String				secretKey	= credentials.GetAWSSecretKey();
	Aws::String				sessionToken= credentials.GetSessionToken();
	Aws::Utils::DateTime	expiration	= GetExparationByValidPeriod(sessionToken, credentials.GetExpiration());

	// Set result buffers
	*ppaccess_key_id	= strdup(accessKeyId.c_str());
	*ppserect_access_key= strdup(secretKey.c_str());
	*ppaccess_token		= strdup(sessionToken.c_str());
	*ptoken_expire		= static_cast<long long>(expiration.Seconds());

	// For debug
	if(Aws::Utils::Logging::LogLevel::Info <= options.loggingOptions.logLevel){
		std::cout << "[s3fsawscred] : Access Key Id = " << accessKeyId	<< std::endl;
		std::cout << "[s3fsawscred] : Secret Key    = " << secretKey	<< std::endl;
		std::cout << "[s3fsawscred] : Session Token = " << sessionToken	<< std::endl;
		std::cout << "[s3fsawscred] : expiration    = " << expiration.ToLocalTimeString(Aws::Utils::DateFormat::ISO_8601) << std::endl;
	}

	if(!*ppaccess_key_id || !*ppserect_access_key || !*ppaccess_token){
		if(pperrstr){
			*pperrstr = strdup("Cloud not allocate memory.");
		}
		if(!*ppaccess_key_id){
			free(*ppaccess_key_id);
			*ppaccess_key_id = NULL;
		}
		if(!*ppserect_access_key){
			free(*ppserect_access_key);
			*ppserect_access_key = NULL;
		}
		if(!*ppaccess_token){
			free(*ppaccess_token);
			*ppaccess_token = NULL;
		}
		*ptoken_expire = 0;

		result = false;
	}

	return result;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab ts=4 fdm=marker
 * vim<600: noexpandtab ts=4
 */
