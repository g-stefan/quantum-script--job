//
// Quantum Script Extension Job
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_HPP
#define QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_HPP

#define QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_ABCD                 2,13,0,24
#define QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_STR                 "2.13.0"
#define QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_STR_BUILD           "24"
#define QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_STR_DATETIME        "2022-01-09 01:13:11"

#ifndef XYO_RC

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB__EXPORT_HPP
#include "quantum-script-extension-job--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Job {
				namespace Version {
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *version();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *build();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *versionWithBuild();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *datetime();
				};
			};
		};
	};
};

#endif
#endif

