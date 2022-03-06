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

#include "config.h"
#include "awscred.h"
#include "awscred_func.h"

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
	if(popts && 0 < strlen(popts)){
		if(0 == strcasecmp(popts, "Off")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Off;
		}else if(0 == strcasecmp(popts, "Fatal")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Fatal;
		}else if(0 == strcasecmp(popts, "Error")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Error;
		}else if(0 == strcasecmp(popts, "Warn")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Warn;
		}else if(0 == strcasecmp(popts, "Info")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
		}else if(0 == strcasecmp(popts, "Debug")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Debug;
		}else if(0 == strcasecmp(popts, "Trace")){
			options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
		}else{
			if(pperrstr){
				*pperrstr = strdup("Unknown option(Aws::Utils::Logging::LogLevel) is specified.");
			}
			return false;
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

	S3fsAWSCredentialsProviderChain	providerChains;
	Aws::SDKOptions&		options		= GetSDKOptions();
	auto					credentials	= providerChains.GetAWSCredentials();
	bool					result		= true;

	// Get credentials
	Aws::String				accessKeyId	= credentials.GetAWSAccessKeyId();
	Aws::String				secretKey	= credentials.GetAWSSecretKey();
	Aws::String				sessionToken= credentials.GetSessionToken();
	Aws::Utils::DateTime	expiration	= credentials.GetExpiration();

	// Set result buffers
	*ppaccess_key_id	= strdup(accessKeyId.c_str());
	*ppserect_access_key= strdup(secretKey.c_str());
	*ppaccess_token		= strdup(sessionToken.c_str());
	*ptoken_expire		= expiration.Millis() / 1000;		// msec to unittime(s)

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
