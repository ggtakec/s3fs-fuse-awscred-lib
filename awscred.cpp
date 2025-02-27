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
static const char S3fsDefaultCredentialsProviderChainTag[]			= "DefaultAWSCredentialsProviderChain";

//----------------------------------------------------------
// Methods : S3fsAWSCredentialsProviderChain
//----------------------------------------------------------
S3fsAWSCredentialsProviderChain::S3fsAWSCredentialsProviderChain(const char* ssoprofile) : Aws::Auth::AWSCredentialsProviderChain()
{
	AddProvider(Aws::MakeShared<Aws::Auth::ProfileConfigFileAWSCredentialsProvider>(S3fsDefaultCredentialsProviderChainTag));
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
