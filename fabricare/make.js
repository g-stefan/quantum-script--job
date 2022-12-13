// Created by Grigore Stefan <g_stefan@yahoo.com>
// Public domain (Unlicense) <http://unlicense.org>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: Unlicense

messageAction("make");

runInPath("source/XYO/QuantumScript.Extension/Job",function(){
	exitIf(Shell.system("file-to-cs --touch=Library.cpp --file-in=Library.js --file-out=Library.Source.cpp --is-string --name=librarySource"));
});

if(!Fabricare.include("make."+Project.make)){
	messageError("Don't know how to make '"+Project.make+"'!");
	exit(1);
};
