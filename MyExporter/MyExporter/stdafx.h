// stdafx.h

#include <assert.h>

//3DXI
#include "IGame.h"
#include "IGameObject.h"
#include "IGameProperty.h"
#include "IGameControl.h"
#include "IGameModifier.h"
#include "IConversionManager.h"
#include "IGameError.h"
#include "IGameFX.h"

//CS
#include "BipedApi.h"

#include <tinyxml/tinystr.h>
#include <tinyxml/tinyxml.h>

//STL
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iosfwd>

#include <stdlib.h>
#include <Windows.h>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { p->Release(); p=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) { if(p) { delete []p; p=NULL; } }
#endif

