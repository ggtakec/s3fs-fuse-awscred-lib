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

#include "awscred.h"

//----------------------------------------------------------
// Variables
//----------------------------------------------------------
static const char S3FS_AWS_ECS_CONTAINER_CREDENTIALS_RELATIVE_URI[]	= "AWS_CONTAINER_CREDENTIALS_RELATIVE_URI";
static const char S3FS_AWS_ECS_CONTAINER_CREDENTIALS_FULL_URI[]		= "AWS_CONTAINER_CREDENTIALS_FULL_URI";
static const char S3FS_AWS_ECS_CONTAINER_AUTHORIZATION_TOKEN[]		= "AWS_CONTAINER_AUTHORIZATION_TOKEN";
static const char S3FS_AWS_EC2_METADATA_DISABLED[]					= "AWS_EC2_METADATA_DISABLED";
static const char S3fsDefaultCredentialsProviderChainTag[]			= "DefaultAWSCredentialsProviderChain";

//----------------------------------------------------------
// Methods : S3fsAWSCredentialsProviderChain
//----------------------------------------------------------
S3fsAWSCredentialsProviderChain::S3fsAWSCredentialsProviderChain() : Aws::Auth::AWSCredentialsProviderChain()
{
	AddProvider(Aws::MakeShared<Aws::Auth::EnvironmentAWSCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
	AddProvider(Aws::MakeShared<Aws::Auth::ProfileConfigFileAWSCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
	AddProvider(Aws::MakeShared<Aws::Auth::ProcessCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
	AddProvider(Aws::MakeShared<Aws::Auth::STSAssumeRoleWebIdentityCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
	AddProvider(Aws::MakeShared<Aws::Auth::STSProfileCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
	AddProvider(Aws::MakeShared<Aws::Auth::SSOCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
	
	//
	// ECS TaskRole Credentials only available when ENVIRONMENT VARIABLE is set
	//
	const auto relativeUri = Aws::Environment::GetEnv(S3FS_AWS_ECS_CONTAINER_CREDENTIALS_RELATIVE_URI);
	AWS_LOGSTREAM_DEBUG(S3fsDefaultCredentialsProviderChainTag, "The environment variable value " << S3FS_AWS_ECS_CONTAINER_CREDENTIALS_RELATIVE_URI << " is " << relativeUri);

	const auto absoluteUri = Aws::Environment::GetEnv(S3FS_AWS_ECS_CONTAINER_CREDENTIALS_FULL_URI);
	AWS_LOGSTREAM_DEBUG(S3fsDefaultCredentialsProviderChainTag, "The environment variable value " << S3FS_AWS_ECS_CONTAINER_CREDENTIALS_FULL_URI << " is " << absoluteUri);

	const auto ec2MetadataDisabled = Aws::Environment::GetEnv(S3FS_AWS_EC2_METADATA_DISABLED);
	AWS_LOGSTREAM_DEBUG(S3fsDefaultCredentialsProviderChainTag, "The environment variable value " << S3FS_AWS_EC2_METADATA_DISABLED << " is " << ec2MetadataDisabled);

	if(!relativeUri.empty()){
		AddProvider(Aws::MakeShared<Aws::Auth::TaskRoleCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag, relativeUri.c_str()));
		AWS_LOGSTREAM_INFO(S3fsDefaultCredentialsProviderChainTag, "Added ECS metadata service credentials provider with relative path: [" << relativeUri << "] to the provider chain.");

	}else if(!absoluteUri.empty()){
		const auto token = Aws::Environment::GetEnv(S3FS_AWS_ECS_CONTAINER_AUTHORIZATION_TOKEN);
		AddProvider(Aws::MakeShared<Aws::Auth::TaskRoleCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag, absoluteUri.c_str(), token.c_str()));

		//DO NOT log the value of the authorization token for security purposes.
		AWS_LOGSTREAM_INFO(S3fsDefaultCredentialsProviderChainTag, "Added ECS credentials provider with URI: [" << absoluteUri << "] to the provider chain with a" << (token.empty() ? "n empty " : " non-empty ") << "authorization token.");

	}else if(Aws::Utils::StringUtils::ToLower(ec2MetadataDisabled.c_str()) != "true"){
		AddProvider(Aws::MakeShared<Aws::Auth::InstanceProfileCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
		AWS_LOGSTREAM_INFO(S3fsDefaultCredentialsProviderChainTag, "Added EC2 metadata service credentials provider to the provider chain.");
	}
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
