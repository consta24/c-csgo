#include "headers.h"
#include "LocalPlayer.h"
#include "Entity.h"
#include "Aimbot.h"
#include "Radar.h"


using namespace hazedumper::signatures;
using namespace hazedumper::netvars;
using namespace std;

int main() {

	srand(static_cast <unsigned> (time(0)));

	SOCKET connection = NULL;
	driver::initialize();
	connection = driver::connect();
	DWORD unload = driver::clean_mmunloadeddrivers(connection);
	DWORD table = driver::clean_piddbcachetable(connection);

	ifstream config;
	ofstream newConfig;

	SetConsoleTitle(L"chrome");
	

	config.open("settings.ini");
	if (config.fail()) {
		cout << "Error opening settings.ini. Creating a new one." << endl;
		newConfig.open("settings.ini");
		newConfig << "--SYNWARE CONFIG--" << endl;
		newConfig << "" << endl;
		newConfig << "//Aimbot settings" << endl;
		newConfig << "" << endl;
		newConfig << "Smooth: 35 { Smooth value should be around 25-50, recommended 35}" << endl;
		newConfig << "Fov: 60" << endl;
		newConfig << "RecoilControl: 1.8 { Recoil value should be around 1.0 ( none ) to 3.0 ( full ), recommended 1.7-2.0 }" << endl;
		newConfig << "PrimaryAimKey: 1 { Left Mouse Button }" << endl;
		newConfig << "SecondAimKey: 86 { 86 = V key // Virtual key codes translated to decimals // 0 - if none }" << endl;
		newConfig << "" << endl;
		newConfig << "//Aimbot to: HEAD NECK BODY" << endl;
		newConfig << "" << endl;
		newConfig << "Rifle: HEAD" << endl;
		newConfig << "AutoSniper: BODY" << endl;
		newConfig << "Pistol: HEAD" << endl;
		newConfig << "Awp: BODY" << endl;
		newConfig << "Shotgun: BODY" << endl;
		newConfig << "" << endl;
		newConfig << "//Other" << endl;
		newConfig << "" << endl;
		newConfig << "PanicKey: 35 { END key is default }" << endl;
		newConfig << "ResolutionX: 1440" << endl;
		newConfig << "ResolutionY: 900" << endl;
		Sleep(2500);
		exit(1);
		return 0;
	}

	char tok[1000];
	char Smooth[30], Fov[30], RecoilControl[30], SecondAimKey[30], PrimaryAimKey[30], Rifle[30], AutoSniper[30], Pistol[30], Awp[30], Shotgun[30], PanicKey[30], ResX[30], ResY[30];
	char RifleBone[10], AutoSniperBone[10], AwpBone[10], ShotgunBone[10], PistolBone[10];
	int SmoothValue, FovValue, PrimaryAimKeyValue, SecondAimKeyValue, PanicKeyValue, ResXValue, ResYValue;
	float RecoilControlValue;

	config.getline(tok, 1000);
	config.getline(tok, 1000);
	config.getline(tok, 1000);
	config >> Smooth >> SmoothValue; config.getline(tok, 1000);
	config >> Fov >> FovValue; config.getline(tok, 1000);
	config >> RecoilControl >> RecoilControlValue; config.getline(tok, 1000);
	config >> PrimaryAimKey >> PrimaryAimKeyValue; config.getline(tok, 1000);
	config >> SecondAimKey >> SecondAimKeyValue; config.getline(tok, 1000);
	config.getline(tok, 1000);
	config.getline(tok, 1000);
	config >> Rifle >> RifleBone; config.getline(tok, 1000);
	config >> AutoSniper >> AutoSniperBone; config.getline(tok, 1000);
	config >> Pistol >> PistolBone; config.getline(tok, 1000);
	config >> Awp >> AwpBone; config.getline(tok, 1000);
	config >> Shotgun >> ShotgunBone; config.getline(tok, 1000);
	config.getline(tok, 1000);
	config.getline(tok, 1000);
	config >> PanicKey >> PanicKeyValue; config.getline(tok, 1000);
	config >> ResX >> ResXValue; config.getline(tok, 1000);
	config >> ResY >> ResYValue; config.getline(tok, 1000);

	int RifleBoneValue, AutoSniperBoneValue, AwpBoneValue, ShotgunBoneValue, PistolBoneValue;

	if (!strcmp(RifleBone, "HEAD"))
		RifleBoneValue = 8;
	else if (!strcmp(RifleBone, "NECK"))
		RifleBoneValue = 7;
	else if (!strcmp(RifleBone, "BODY"))
		RifleBoneValue = 5;

	if (!strcmp(AutoSniperBone, "HEAD"))
		AutoSniperBoneValue = 8;
	else if (!strcmp(AutoSniperBone, "NECK"))
		AutoSniperBoneValue = 7;
	else if (!strcmp(AutoSniperBone, "BODY"))
		AutoSniperBoneValue = 5;

	if (!strcmp(AwpBone, "HEAD"))
		AwpBoneValue = 8;
	else if (!strcmp(AwpBone, "NECK"))
		AwpBoneValue = 7;
	else if (!strcmp(AwpBone, "BODY"))
		AwpBoneValue = 5;

	if (!strcmp(ShotgunBone, "HEAD"))
		ShotgunBoneValue = 8;
	else if (!strcmp(ShotgunBone, "NECK"))
		ShotgunBoneValue = 7;
	else if (!strcmp(ShotgunBone, "BODY"))
		ShotgunBoneValue = 5;

	if (!strcmp(PistolBone, "HEAD"))
		PistolBoneValue = 8;
	else if (!strcmp(PistolBone, "NECK"))
		PistolBoneValue = 7;
	else if (!strcmp(PistolBone, "BODY"))
		PistolBoneValue = 5;
	
	cout << Smooth << SmoothValue << endl;
	cout << Fov << FovValue << endl;
	cout << RecoilControl << RecoilControlValue << endl;
	cout << PrimaryAimKey << PrimaryAimKeyValue << endl;
	cout << SecondAimKey << SecondAimKeyValue << endl;
	cout << Rifle << RifleBone << " " << RifleBoneValue << endl;
	cout << AutoSniper << AutoSniperBone << " " << AutoSniperBoneValue << endl;
	cout << Pistol << PistolBone << " " << PistolBoneValue << endl;
	cout << Awp << AwpBone << " " << AwpBoneValue << endl;
	cout << Shotgun << ShotgunBone << " " << ShotgunBoneValue << endl;
	cout << PanicKey << PanicKeyValue << endl;
	cout << ResX << ResXValue << endl;
	cout << ResY << ResYValue << endl;

	Sleep(2500);
	system("cls");

	if (table && unload) {
		cout << "Connection established." << endl;
		Sleep(2000);
		system("cls");
	}
	else {
		cout << "Connection failed." << endl;
		Sleep(2000);
		system("cls");
	}

	DWORD procID, moduleBase, engineBase;
	cout << "Process ID: ";
	cin >> procID;
	Sleep(500);

	cout << "Finding process info...";
	while (true) {
		moduleBase = driver::get_process_client_address(connection, procID);
		engineBase = driver::get_process_engine_address(connection, procID);
		if (moduleBase != 0 && engineBase != 0) {
			cout << "Process found";
			break;
		}
	}

	Sleep(2500);
	//FreeConsole();

	LocalPlayer player;

	bool bAimbot = true;
	bool bGlow = true;
	DWORD lastClosestEnemy = NULL;
	DWORD closestEnemy = NULL;
	float bright = 0.f;

	while (true) {
		DWORD localPlayer = driver::read<DWORD>(connection, procID, moduleBase + dwLocalPlayer);

		Radar(connection, procID, moduleBase);

		if (bAimbot) {
			if (GetAsyncKeyState(PrimaryAimKeyValue) & 0x8000 || GetAsyncKeyState(SecondAimKeyValue) & 0x8000) {
				closestEnemy = FindClosestEnemy(connection, procID, localPlayer, moduleBase, engineBase, ResXValue, ResYValue);
				if (lastClosestEnemy != closestEnemy && lastClosestEnemy != NULL) {
					int random = rand() % (450 + 1 - 200) + 200;
					Sleep(random);
				}
				Aimbot(connection, procID, closestEnemy, localPlayer, moduleBase, engineBase, SmoothValue, FovValue, RecoilControlValue,
					RifleBoneValue, AwpBoneValue, ShotgunBoneValue, AutoSniperBoneValue, PistolBoneValue, ResXValue, ResYValue);
				lastClosestEnemy = closestEnemy;
			}
			else {
				lastClosestEnemy = NULL;
			}
		}

		if (GetAsyncKeyState(PanicKeyValue)) {
			AllocConsole();
			Sleep(1000);
			break;
		}
		Sleep(0.5);
	}
	

	driver::disconnect(connection);
	driver::deinitialize();
	config.close();
	return 0;
}