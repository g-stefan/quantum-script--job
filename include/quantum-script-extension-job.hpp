//
// Quantum Script Extension Job
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_HPP
#define QUANTUM_SCRIPT_EXTENSION_JOB_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB__EXPORT_HPP
#include "quantum-script-extension-job--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_COPYRIGHT_HPP
#include "quantum-script-extension-job-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_LICENSE_HPP
#include "quantum-script-extension-job-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_VERSION_HPP
#include "quantum-script-extension-job-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Job {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

