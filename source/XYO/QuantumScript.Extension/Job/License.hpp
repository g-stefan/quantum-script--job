// Quantum Script Extension Job
// Copyright (c) 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2016-2024 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JOB_LICENSE_HPP
#define XYO_QUANTUMSCRIPT_EXTENSION_JOB_LICENSE_HPP

#ifndef XYO_QUANTUMSCRIPT_EXTENSION_JOB_DEPENDENCY_HPP
#	include <XYO/QuantumScript.Extension/Job/Dependency.hpp>
#endif

namespace XYO::QuantumScript::Extension::Job::License {

	XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT std::string license();
	XYO_QUANTUMSCRIPT_EXTENSION_JOB_EXPORT std::string shortLicense();

};

#endif
