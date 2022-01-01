//
// Quantum Script Extension Job
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-job-license.hpp"
#include "quantum-script-extension-job.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_NO_VERSION
#include "quantum-script-extension-job-version.hpp"
#endif

#include "quantum-script-extension-job.src"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace Job {

				using namespace XYO;
				using namespace Quantum::Script;

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("Job", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "Job\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "Job");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_JOB_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::Job::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX(extensionJobSource);
				};

			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_JOB_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::Job::initExecutive(executive, extensionId);
};
#endif

