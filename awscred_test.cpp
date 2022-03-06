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

#include <time.h>
#include <iostream>

#include "awscred_func.h"

int main(int argc, char** argv)
{
	char*	perrstr = NULL;

	std::cout << "[awscred_test] Start test for s3fsawscred.so" << std::endl;
	std::cout << std::endl;

	//
	// Test : InitS3fsCredential
	//
	std::cout << "  [Function] InitS3fsCredential" << std::endl;
	if(!InitS3fsCredential("Debug", &perrstr)){
		std::cerr << "     [ERROR] Could not initialize s3fsawscred.so : " << (perrstr ? perrstr : "unknown") << std::endl;
		if(perrstr){
			free(perrstr);
		}
		exit(EXIT_FAILURE);
	}
	std::cout << "     [Succeed]" << std::endl;
	std::cout << std::endl;

	//
	// Test : VersionS3fsCredential
	//
	std::cout << "  [Function] VersionS3fsCredential - short" << std::endl;
	const char* pVersion = VersionS3fsCredential(false);
	if(!pVersion){
		std::cerr << "     [ERROR] Version string(not detail) is NULL." << std::endl;
		FreeS3fsCredential(&perrstr);
		if(perrstr){
			free(perrstr);
		}
		exit(EXIT_FAILURE);
	}
	std::cout << "     [Succeed] " << pVersion << std::endl;
	std::cout << std::endl;

	std::cout << "  [Function] VersionS3fsCredential - detail" << std::endl;
	pVersion = VersionS3fsCredential(true);
	if(!pVersion){
		std::cerr << "     [ERROR] Version string(detail) is NULL." << std::endl;
		FreeS3fsCredential(&perrstr);
		if(perrstr){
			free(perrstr);
		}
		exit(EXIT_FAILURE);
	}
	std::cout << "     [Succeed] " << std::endl;
	std::cout << pVersion;
	std::cout << std::endl;

	//
	// Test : UpdateS3fsCredential
	//
	char*		paccess_key_id		= NULL;
	char*		pserect_access_key	= NULL;
	char*		paccess_token		= NULL;
	long long	token_expire		= 0;

	std::cout << "  [Function] UpdateS3fsCredential" << std::endl;
	if(!UpdateS3fsCredential(&paccess_key_id, &pserect_access_key, &paccess_token, &token_expire, &perrstr)){
		std::cerr << "     [ERROR] Could not get Credential for AWS : " << (perrstr ? perrstr : "unknown") << std::endl;
		if(perrstr){
			free(perrstr);
		}
		FreeS3fsCredential(&perrstr);
		if(perrstr){
			free(perrstr);
		}
		exit(EXIT_FAILURE);
	}
	std::cout << "     [Succeed] Credential = {"									<< std::endl;
	std::cout << "                 AWS Access Key Id    = " << paccess_key_id		<< std::endl;
	std::cout << "                 AWS Secret Key       = " << pserect_access_key	<< std::endl;
	std::cout << "                 AWS Session Token    = " << paccess_token		<< std::endl;
	std::cout << "                 Expiration(unixtime) = " << token_expire			<< std::endl;
	std::cout << "               }"													<< std::endl;
	std::cout << std::endl;

	//
	// Test : FreeS3fsCredential
	//
	std::cout << "  [Function] FreeS3fsCredential" << std::endl;
	if(!FreeS3fsCredential(&perrstr)){
		std::cerr << "     [ERROR] Could not uninitialize s3fsawscred.so : " << (perrstr ? perrstr : "unknown") << std::endl;
		if(perrstr){
			free(perrstr);
		}
		exit(EXIT_FAILURE);
	}
	std::cout << "     [Succeed]" << std::endl;
	std::cout << std::endl;

	exit(EXIT_SUCCESS);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noexpandtab sw=4 ts=4 fdm=marker
 * vim<600: noexpandtab sw=4 ts=4
 */
