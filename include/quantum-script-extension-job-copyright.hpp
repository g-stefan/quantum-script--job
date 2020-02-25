//
// Quantum Script Extension Job
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_COPYRIGHT_HPP
#define QUANTUM_SCRIPT_EXTENSION_JOB_COPYRIGHT_HPP

#define QUANTUM_SCRIPT_EXTENSION_JOB_COPYRIGHT            "Copyright (c) Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_JOB_PUBLISHER            "Grigore Stefan"
#define QUANTUM_SCRIPT_EXTENSION_JOB_COMPANY              QUANTUM_SCRIPT_EXTENSION_JOB_PUBLISHER
#define QUANTUM_SCRIPT_EXTENSION_JOB_CONTACT              "g_stefan@yahoo.com"
#define QUANTUM_SCRIPT_EXTENSION_JOB_FULL_COPYRIGHT       QUANTUM_SCRIPT_EXTENSION_JOB_COPYRIGHT " <" QUANTUM_SCRIPT_EXTENSION_JOB_CONTACT ">"

#ifndef XYO_RC

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB__EXPORT_HPP
#include "quantum-script-extension-job--export.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Job {
				namespace Copyright {
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *copyright();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *publisher();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *company();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *contact();
					QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT const char *fullCopyright();
				};
			};
		};
	};
};

#endif
#endif
