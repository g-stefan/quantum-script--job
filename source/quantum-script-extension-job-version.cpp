//
// Quantum Script Extension Job
//
// Copyright (c) 2020-2021 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include "quantum-script-extension-job-version.hpp"

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Job {
				namespace Version {

					static const char *version_ = "2.3.0";
					static const char *build_ = "21";
					static const char *versionWithBuild_ = "2.3.0.21";
					static const char *datetime_ = "2021-11-15 16:51:51";

					const char *version() {
						return version_;
					};
					const char *build() {
						return build_;
					};
					const char *versionWithBuild() {
						return versionWithBuild_;
					};
					const char *datetime() {
						return datetime_;
					};

				};
			};
		};
	};
};



