// world.h: interface for the World class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLD_H__B7E5020B_10F6_4DFA_8C5B_412E13564A14__INCLUDED_)
#define AFX_WORLD_H__B7E5020B_10F6_4DFA_8C5B_412E13564A14__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../pfc.h"
#include "Group.h"
#include "Scene.h"

typedef struct {
	int nGroups;
	int nCharacters;
	int fps;
	int rps;
	int rpsAux;
} Tstats;

class World  
{

public:

	CList<Group,Group&> groups;
	CList<Group,Group&> enemies;
	Scene* scene;

	Tstats stats;

public:
	World();
	virtual ~World();
	void Initialize();

	void Update(unsigned long elapsedTime);
	void UnselectAllGroups();

	void ReadScene(CString fName);

	void RegroupSelectedGroup();
};

#endif // !defined(AFX_WORLD_H__B7E5020B_10F6_4DFA_8C5B_412E13564A14__INCLUDED_)
