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

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentials.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/core/auth/STSCredentialsProvider.h>
#include <aws/core/auth/SSOCredentialsProvider.h>
#include <aws/identity-management/auth/STSProfileCredentialsProvider.h>
#include <aws/core/platform/Environment.h>
#include <aws/core/utils/DateTime.h>
#include <aws/core/utils/logging/LogMacros.h>

//----------------------------------------------------------
// Class S3fsAWSCredentialsProviderChain
//----------------------------------------------------------
// [NOTE]
// This class for STS, see https://github.com/aws/aws-sdk-cpp/issues/150#issuecomment-538548438
//
class S3fsAWSCredentialsProviderChain : public Aws::Auth::AWSCredentialsProviderChain
{
	public:
		S3fsAWSCredentialsProviderChain(const char* ssoprofile = nullptr);
};

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
