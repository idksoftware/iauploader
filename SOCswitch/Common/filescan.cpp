// filescan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "TargetsList.h"
#include "VirtualFileSystem.h"
#include "SyncTool.h"
#include "SyncWorker.h"

int  main()
{
	SyncWorker syncWorker;
	syncWorker.Start();
	return 0;
	/*
VirtualFileSystem virtualFileSystem;
virtualFileSystem.insertDirectory("U:\\-\\aalto\\Development\\jsonCmd_2");
virtualFileSystem.insertFile("U:\\-\\aalto\\Development\\jsonCmd_2\\input.txt");

std::vector < std::string> list;
	if (virtualFileSystem.listFolder(list, "U:\\-\\aalto\\Development\\jsonCmd_2") != true)
{
	return -1;
}
	for (auto ii : list)
	{
		std::string item = ii;
		printf("%s\n", item.c_str());
	}
virtualFileSystem.findDirectory("U:\\-\\aalto\\Development\\jsonCmd_2");
*/
	/*
	SyncTool syncTool;
	TargetsList targetsList(syncTool);

	targetsList.process("U:\\-\\aalto\\Development\\jsonCmd_2");

	JobList& joblist = syncTool.getJobList();

	for (auto ii : joblist)
	{
		SyncJob job = *ii;
		printf("%s\n", job.getFilepath());
	}
	*/
	/*
	std::shared_ptr<FolderDir> fd = targetsList.getVFS();
	std::shared_ptr<VirtualFileSystem> vfs = fd->getVFS();
	std::vector < std::string> list;
	if (vfs->listFolder(list, "U:\\-\\aalto\\Development\\jsonCmd_2") != true)
	{
		return -1;
	}
	for (auto ii : list)
	{
		std::string item = ii;
		printf("%s\n", item.c_str());
	}


	if (vfs->listFolder(list, "U:\\-\\aalto\\Development\\jsonCmd_2\\jsonCmd") != true)
	{
		return -1;
	}
	for (auto ii : list)
	{
		std::string item = ii;
		printf("%s\n", item.c_str());
	}
	*/
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
