#include"mainwindowui.h"

static bool show_scene_setting_view = false;
static bool show_scene_resource_view = true;
static bool show_resource_list_view = true;
static bool show_unity_load_view = false;
static bool show_camera_setting_view = false;
static bool show_point_light_setting_view = false;
static bool show_direction_light_setting_view = false;
static bool show_render_setting_view = true;
static bool show_camera_change_view = false;
static bool show_materila_change_view = false;
static bool Show_Scene_Hierarchy = true;
static bool Show_Texture_Resource = false;
static bool Show_Object_Resource = false;
static bool Show_Material_Resource = false;
static bool Show_Light_Resource = false;
static bool Show_Unity_Property = false;
static bool Show_RayTracer = false;
static int SelectUnityID = -1;

void MainWindowUI(WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng, RayTraceManager& rayMng, 
	PathTracerManager& optixPtMng, GradientPathTracer& optixGptMng, PreIntergratManager& preInterMng, bool *p_open)
{

	if (show_scene_resource_view)	ScenenResourceView(winDev, basicMng, renderMng, &show_scene_resource_view);
	//if (show_resource_list_view)	ResourceListView(winDev, basicMng, &show_resource_list_view);
	if (show_render_setting_view) RenderSettingView(winDev, basicMng, renderMng, &show_render_setting_view);
	if (Show_Scene_Hierarchy) SceneHierarchy(&Show_Scene_Hierarchy, basicMng);
	if (Show_Texture_Resource) ShowTextureResource(&Show_Texture_Resource, basicMng);
	if (Show_Object_Resource) ShowObjectResource(&Show_Object_Resource, basicMng);
	if (Show_Material_Resource) ShowMaterialResource(&Show_Material_Resource, basicMng);
	if (Show_Light_Resource) ShowLightResource(&Show_Light_Resource, basicMng);
	if (Show_Unity_Property) ShowUnityProperty(&Show_Unity_Property,winDev,basicMng, renderMng, SelectUnityID);

	if (Show_RayTracer) ShowRayTracer(&Show_RayTracer, basicMng, renderMng, optixGptMng);

	// Main Menu for SeaAsia
	if (ImGui::BeginMainMenuBar())
	{
		// File menu
		if (ImGui::BeginMenu("File"))
		{
			//build new project
			if (ImGui::MenuItem("New")) {}
			//open a project
			if (ImGui::MenuItem("Open", "Ctrl+O")) {}
			//open a project used at last time
			if (ImGui::BeginMenu("Open Recent"))
			{
				if (ImGui::BeginMenu("More..")) { ImGui::EndMenu(); }
				ImGui::EndMenu();
			}

			//save the project
			if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			//save this project in other file and name
			if (ImGui::MenuItem("Save As..")) {}

			ImGui::Separator();

			//system setting
			if (ImGui::BeginMenu("Options"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem("Checked")) {}
			//save and close SeaAsia
			if (ImGui::MenuItem("Quit", "Alt+F4")) {}
			ImGui::EndMenu();
		}

		//Eidt menu
		if (ImGui::BeginMenu("Edit"))
		{
			//return to the last step
			if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			//go to the nest step
			if (ImGui::MenuItem("Redo", "CTRL+Y")) {}  // Disabled item
			ImGui::Separator();

			//operator to resource: cut, copy and paste
			if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			ImGui::EndMenu();
		}

		//View
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::BeginMenu("Scene Resource View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &show_scene_resource_view));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Texture View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &Show_Texture_Resource));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Object View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &Show_Object_Resource));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Material View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &Show_Material_Resource));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Light View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &Show_Light_Resource));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Scene Hierarchy View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &Show_Scene_Hierarchy));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("skybox View"))
			{
				if (ImGui::BeginPopupModal("skybox editor"))
				{
					ImGui::Text("choose a texture!");

					static int selected = 0;
					ImGui::BeginChild("left pane", ImVec2(0, 0), true);
					for (int ind = 0; ind < basicMng.textureManager.endTextureId; ind++)
					{
						char label[128];
						string texN = "%d ,";
						texN += basicMng.textureManager.texNames[ind];
						sprintf_s(label, texN.c_str(), ind);
						if (ImGui::Selectable(label, selected == ind))
							selected = ind;
					}
					ImGui::EndChild();

					if (ImGui::Button("OK"))
					{
						renderMng.skybox.ChooseTexture(basicMng.textureManager, selected);
						ImGui::CloseCurrentPopup();
					}
					if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				if (ImGui::Button("Change skybox", ImVec2(150, 20)))
				{
					ImGui::OpenPopup("skybox editor");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		//Resource manager menu
		if (ImGui::BeginMenu("Resource Manager")) { ImGui::EndMenu(); }

		//Low-Level pipeline Render 
		if (ImGui::BeginMenu("Low-Level Render "))
		{
			if (ImGui::MenuItem("Open Dx11 Render Setting", NULL, &show_render_setting_view));
			ImGui::EndMenu();
		}

		//ray trace render
		if (ImGui::BeginMenu("Ray Trace Render "))
		{
			if (ImGui::MenuItem("Show the View", NULL, &Show_RayTracer));
			ImGui::EndMenu();
		}

		//ray trace render
		if (ImGui::BeginMenu("PreIntergrate Env Light "))
		{
			if (ImGui::Button("bake", ImVec2(150, 20)))
			{
				preInterMng.Bake(basicMng, renderMng);
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

}

static void ScenenResourceView(WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng, bool *p_open)
{

	if (show_scene_setting_view) SceneSettingView(basicMng, &show_scene_setting_view);
	if (show_unity_load_view)	UnityLoadingView(basicMng, renderMng, &show_unity_load_view);
	if (show_camera_setting_view) CameraSettingView(basicMng, &show_camera_setting_view);
	if (show_direction_light_setting_view) DirectionLightSettingView(basicMng, renderMng, &show_direction_light_setting_view);
	if (show_point_light_setting_view)	PointLightSettingView(basicMng, &show_point_light_setting_view);
	if (show_camera_change_view)	CameraCangeView(basicMng, &show_camera_change_view);
	if (show_materila_change_view)	MaterialChangeView(basicMng, renderMng, &show_materila_change_view);

	RECT rect;
	GetClientRect(winDev.hwnd, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(w*0.15, h*0.77));
	/*ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(-1, -1));*/

	if (!ImGui::Begin("Scene Resource View", p_open, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		ImGui::End();
		return;
	}

	//show scene tree
	int i = basicMng.sceneManager.currentSceneId;
	if (ImGui::TreeNode((void*)(intptr_t)i, "Scene %d", i))
	{
		//show unity tree
		if (ImGui::TreeNode("UnityList"))
		{
			if (basicMng.sceneManager.sceneList[i].unityList == NULL)
			{
				ImGui::Text("Scene don't create Unity List");
			}
			else
			{
				if (basicMng.sceneManager.sceneList[i].endUnityId == 0)
				{
					ImGui::Text("No Unity is existence!");
				}

				for (int j = 0; j < basicMng.sceneManager.sceneList[i].endUnityId; j++)
				{
					if (ImGui::TreeNode((void*)(intptr_t)j, "Unity %d", j))
					{
						Unity &unity = basicMng.sceneManager.sceneList[i].unityList[j];
						basicMng.sceneManager.sceneList[i].currentUnityId = j;

						ImGui::Text("Triangle Mesh ID: %d", unity.objId);
						ImGui::Text("Material Number: %d", unity.materialNum);

						if (ImGui::Button("Reverse normal"))
						{
							renderMng.ReverseUnityNormal(basicMng, unity);
						}

						if (ImGui::Button("Reverse normal Z axis"))
						{
							renderMng.ReverseUnityNormalZaxis(basicMng, unity);
						}
						if (ImGui::Button("Change Material"))
						{
							basicMng.materialsManager.currentMtlId = unity.MaterialsIdIndex[0];
							show_materila_change_view = true;
						}

						ImGui::TreePop();
					}
				}

				if (ImGui::Button("Add new Unity", ImVec2(-1, 20)))
				{
					if (basicMng.sceneManager.sceneList[i].endUnityId >= basicMng.sceneManager.sceneList[i].maxUnityNum)
					{
						ImGui::SetNextWindowSize(ImVec2(100, 450), ImGuiSetCond_FirstUseEver);
						if (!ImGui::Begin("error"))
						{
							ImGui::End();
							return;
						}
						ImGui::Text("Unity List is full!");
						ImGui::End();

					}
					else
					{
						show_unity_load_view = true;
					}

				}
			}

			ImGui::TreePop();
		}

		//show camera tree
		if (ImGui::TreeNode("CameraList"))
		{

			if (basicMng.sceneManager.sceneList[i].cameraList == NULL)
			{
				ImGui::Text("Scene don't create Camera List");
			}
			else
			{
				if (basicMng.sceneManager.sceneList[i].endCameraId == 0)
				{
					ImGui::Text("No camera is existence");
				}

				for (int k = 0; k < basicMng.sceneManager.sceneList[i].endCameraId; k++)
				{
					if (ImGui::TreeNode((void*)(intptr_t)k, "Camera %d", k))
					{
						DxCamera &camera = basicMng.sceneManager.sceneList[i].cameraList[k];
						basicMng.sceneManager.sceneList[i].currentCameraId = k;

						ImGui::Text("Camera position : %f %f %f", camera.eye.x, camera.eye.y, camera.eye.z);
						ImGui::Text("Camera look at : %f %f %f", camera.at.x, camera.at.y, camera.at.z);
						ImGui::Text("Camera up : %f %f %f", camera.up.x, camera.up.y, camera.up.z);
						ImGui::Text("Camera near face: %f", camera.cNear);
						ImGui::Text("Camera far face: %f", camera.cFar);
						ImGui::Text("Camera view angle: %f", camera.aspect);
						if (ImGui::Button("Chang", ImVec2(100, 20)))
						{
							show_camera_change_view = true;
						}

						ImGui::TreePop();
					}
				}

				if (ImGui::Button("Add new camera", ImVec2(-1, 20)))
				{
					if (basicMng.sceneManager.sceneList[i].endCameraId >= basicMng.sceneManager.sceneList[i].maxCameraNum)
					{
						ImGui::SetNextWindowSize(ImVec2(100, 450), ImGuiSetCond_FirstUseEver);
						if (!ImGui::Begin("error"))
						{
							ImGui::End();
							return;
						}
						ImGui::Text("Camera List is full!");
						ImGui::End();
					}
					else
					{
						show_camera_setting_view = true;
					}
				}
			}


			ImGui::TreePop();
		}


		//show light
		//direction light
		if (ImGui::TreeNode("DirectionLight"))
		{
			if (basicMng.sceneManager.sceneList[i].dlList == NULL)
			{
				ImGui::Text("Scene don't create Diraction Light");
			}
			else
			{
				if (basicMng.sceneManager.sceneList[i].endDlId == 0)
				{
					ImGui::Text("No Direction Light is existence!");
				}
				//show direction light information
				for (int l = 0; l < basicMng.sceneManager.sceneList[i].endDlId; l++)
				{
					if (ImGui::TreeNode((void*)(intptr_t)l, "DirectionLight %d", l))
					{
						basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentDlId = l;

						ImGui::Text("Direction: f% f% f%",
							basicMng.sceneManager.sceneList[i].dlList[l].Dir.x,
							basicMng.sceneManager.sceneList[i].dlList[l].Dir.y,
							basicMng.sceneManager.sceneList[i].dlList[l].Dir.z);
						ImGui::Text("Color: f% f% f%",
							basicMng.sceneManager.sceneList[i].dlList[l].Color.x,
							basicMng.sceneManager.sceneList[i].dlList[l].Color.y,
							basicMng.sceneManager.sceneList[i].dlList[l].Color.z);
						ImGui::TreePop();
					}
				}

				//add new light
				if (ImGui::Button("Add new directionLight", ImVec2(-1, 20)))
				{
					if (basicMng.sceneManager.sceneList[i].endDlId >= basicMng.sceneManager.sceneList[i].maxDlNum)
					{
						ImGui::SetNextWindowSize(ImVec2(100, 450), ImGuiSetCond_FirstUseEver);
						if (!ImGui::Begin("error"))
						{
							ImGui::End();
							return;
						}
						ImGui::Text("Direction Light List is full!");
						ImGui::End();
					}
					else
					{
						show_direction_light_setting_view = true;
					}
				}
			}
			ImGui::TreePop();
		}


		//show point light
		if (ImGui::TreeNode("PointLight"))
		{
			if (basicMng.sceneManager.sceneList[i].plList == NULL)
			{
				ImGui::Text("Scene don't create Point Light");
			}
			else
			{
				if (basicMng.sceneManager.sceneList[i].endPlId == 0)
				{
					ImGui::Text("No Point Light is existence!");
				}

				// show point light information
				for (int m = 0; m < basicMng.sceneManager.sceneList[i].endPlId; m++)
				{
					if (ImGui::TreeNode((void*)(intptr_t)m, "PointLight %d", m))
					{
						ImGui::Text("Position: %f %f %f",
							basicMng.sceneManager.sceneList[i].plList[m].Pos.x,
							basicMng.sceneManager.sceneList[i].plList[m].Pos.y,
							basicMng.sceneManager.sceneList[i].plList[m].Pos.z);
						ImGui::Text("Color: %f %f %f",
							basicMng.sceneManager.sceneList[i].plList[m].Color.x,
							basicMng.sceneManager.sceneList[i].plList[m].Color.y,
							basicMng.sceneManager.sceneList[i].plList[m].Color.z);
						ImGui::TreePop();
					}
				}

				//add new point light
				if (ImGui::Button("Add new pointlight", ImVec2(-1, 20)))
				{
					if (basicMng.sceneManager.sceneList[i].endPlId >= basicMng.sceneManager.sceneList[i].maxPlNum)
					{
						ImGui::SetNextWindowSize(ImVec2(100, 450), ImGuiSetCond_FirstUseEver);
						if (!ImGui::Begin("error"))
						{
							ImGui::End();
							return;
						}
						ImGui::Text("Point Light List is full!");
						ImGui::End();
					}
					else
					{
						show_point_light_setting_view = true;
					}
				}
			}



			ImGui::TreePop();
		}

		ImGui::TreePop();
	}

	//scene init setting
	if (basicMng.sceneManager.sceneList[i].unityList == NULL
		&& basicMng.sceneManager.sceneList[i].cameraList == NULL
		&& basicMng.sceneManager.sceneList[i].dlList == NULL
		&& basicMng.sceneManager.sceneList[i].plList == NULL)
	{
		if (ImGui::Button("Scene Setup", ImVec2(-1, 20)))
		{
			show_scene_setting_view = true;
		}
	}
	else
	{
		if (ImGui::Button("Scene Cleanup", ImVec2(-1, 20)))
		{

		}
	}

	ImGui::End();
}

static void ResourceListView(WindowsDevice & winDev, BasicManager &basicMng, bool *p_open)
{
	RECT rect;
	GetClientRect(winDev.hwnd, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	ImGui::SetNextWindowPos(ImVec2(0, h*0.8));
	ImGui::SetNextWindowSize(ImVec2(w, h*0.2));
	/*ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(-1, -1));*/

	if (!ImGui::Begin("Key Frame View", p_open, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		ImGui::End();
		return;
	}
	
}

static void SceneSettingView(BasicManager &basicMng, bool *p_open)
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Scene SetUp", p_open))
	{
		ImGui::End();
		return;
	}

	static int unityNum = 0;
	static int cameraNum = 0;
	static int plNum = 0;
	static int dlNum = 0;

	ImGui::PushItemWidth(50);
	ImGui::DragInt("Max Unity Number", &unityNum, 0.02f, 1, 50000, NULL);
	ImGui::DragInt("Max Camera Number", &cameraNum, 0.02f, 1, 500, NULL);
	ImGui::DragInt("Max Direction Light Number", &dlNum, 0.02f, 1, 50000, NULL);
	ImGui::DragInt("Max Point Light Number", &plNum, 0.02f, 1, 50000, NULL);
	ImGui::PopItemWidth();

	if (ImGui::Button("OK", ImVec2(-1, 20)))
	{
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].setUp(unityNum, cameraNum, plNum, dlNum);
		show_scene_setting_view = false;
	}
	if (ImGui::Button("Add Defeat Number", ImVec2(-1, 20)))
	{
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].setUp(10000, 10, 10, 10);
		show_scene_setting_view = false;
	}
	ImGui::End();
}

static void UnityLoadingView(BasicManager &basicMng, LowLevelRendermanager &renderMng, bool *p_open)
{

	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Unity Load", p_open))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("choose 3D model type: ");
	ImGui::SameLine();
	ImGui::PushItemWidth(120);
	static int type = 1;
	ImGui::Combo("model type", &type, ".obj\0.x\0.fbx\0.pmd\0.3ds\0\0");

	if (type == 0)
	{
		static wstring objName;
		static wstring mtlName;
		static wstring textureName;
		static string error = "no error!";

		static char str1[128] = "ball.obj";
		static char str2[128] = "ball.mtl";
		static char str3[128] = "ball.bmp";


		ImGui::InputText("Input OBJ File Name", str1, 128);
		objName = CharToWstring(str1);
		ImGui::InputText("Input Material File Name", str2, 128);
		mtlName = CharToWstring(str2);
		ImGui::InputText("Input Texture File Name", str3, 128);
		textureName = CharToWstring(str3);

		static int objType = 0;
		ImGui::Combo("Object type", &objType, "Triangle\0Sphere\0RectangleType\0EmissiveType\0\0");

		if (ImGui::BeginPopupModal("obj error"))
		{
			ImGui::Text("can't load obj file!");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("mtl error"))
		{
			ImGui::Text("can't load material file");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("texture error"))
		{
			ImGui::Text("can't load texture file");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::Button("OK", ImVec2(-1, 20)))
		{
			ObjectType otype = TriangleType;
			if (objType == 0)	otype = TriangleType;
			if (objType == 1)	otype = SphereType;
			if (objType == 2)	otype = RectangleType;
			if (objType == 3)	otype = EmissiveType;

			if (renderMng.LoadUnityFromObjFile(objName, mtlName, textureName,
				basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId], basicMng, otype))
			{	
				basicMng.sceneManager.ComputeSceneBounds(basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId],
					basicMng.objManager);
				basicMng.textureManager.DxSetSamplerDesc(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_TEXTURE_ADDRESS_WRAP,
					D3D11_COMPARISON_NEVER, 0, D3D11_FLOAT32_MAX, NULL, NULL);
				basicMng.textureManager.DxSetSamplerState(basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].unityList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentUnityId].samplerStateId,
					basicMng.dxDevice);
				show_unity_load_view = false;
			}
			else if (basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].unityList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endUnityId].objId == -1)
			{
				ImGui::OpenPopup("obj error");

			}

			else if (basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].unityList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endUnityId].materialNum == 0)
			{
				ImGui::OpenPopup("mtl error");
			}

			else if (textureName[0] != NULL && basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].unityList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endUnityId].textureId == -1)
			{
				ImGui::OpenPopup("texture error");
			}
			else
			{
				ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
				ImGui::Begin("error!");
				ImGui::Text("other error!");
				ImGui::End();
			}

		}
		ImGui::Text(error.c_str());
	}

	if (type == 2)
	{
		static wstring objName;
		static wstring mtlName;
		static wstring textureName;
		static string error = "no error!";

		static char str1[128] = "h.fbx";

		ImGui::InputText("Input OBJ File Name", str1, 128);
		objName = CharToWstring(str1);

		if (ImGui::BeginPopupModal("fbx error"))
		{
			ImGui::Text("can't load fbx file!");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::Button("OK", ImVec2(-1, 20)))
		{
			if (renderMng.LoadUnityFromFBXFile(str1, basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId], basicMng))
			{
				show_unity_load_view = false;
				basicMng.sceneManager.ComputeSceneBounds(basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId],
					basicMng.objManager);
			}
			else 
			{
				ImGui::OpenPopup("fbx error");

			}

		}
		ImGui::Text(error.c_str());
	}

	ImGui::End();
}

static void CameraSettingView(BasicManager &basicMng, bool *p_open)
{

	static float position[3] = { 0,0,0 };
	static float lookat[3] = { 0,0,1.f };
	static float up[3] = { 0,1.f,0 };
	static float cnear = 0.1f;
	static float cfar = 10000.f;
	static float aspect = 60;
	static float cwidth = GetSystemMetrics(SM_CXSCREEN);
	static float cheight = GetSystemMetrics(SM_CYSCREEN);

	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Camera Setting", p_open))
	{
		ImGui::End();
		return;
	}

	if (position[0] == lookat[0] && position[1] == lookat[1] && position[2] == lookat[2])
	{
		lookat[0] = position[0] + 1.f;
		lookat[1] = position[1] + 1.f;
		lookat[2] = position[2] + 1.f;
	}

	ImGui::InputFloat3("camera position", position);
	ImGui::InputFloat3("camera loook-at", lookat);

	if (up[0] == 0 && up[1] == 0 && up[2] == 0)
	{
		up[1] = 1.f;
	}
	ImGui::InputFloat3("camera up", up);

	if (cfar == cnear)	cfar = cnear + 0.1;
	ImGui::InputFloat("camera near face", &cnear);
	ImGui::InputFloat("camera farface", &cfar);

	if (aspect == 0.f)	aspect = 0.01;
	ImGui::InputFloat("camera visual angle", &aspect);

	if (cwidth == 0.f)	cwidth = 0.01;
	ImGui::InputFloat("camera width", &cwidth);

	if (cheight == 0.f)	cheight = 0.01;
	ImGui::InputFloat("camera height", &cheight);


	if (ImGui::Button("OK", ImVec2(-1, 20)))
	{
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentCameraId = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endCameraId;
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].cameraList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endCameraId].Init(
			position[0], position[1], position[2], lookat[0], lookat[1], lookat[2], up[0], up[1], up[2], cnear, cfar, aspect*3.1415f / 360.f, cwidth, cheight);
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endCameraId++;
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].cameraNum++;
		show_camera_setting_view = false;
	}


	ImGui::End();

}

static void CameraCangeView(BasicManager &basicMng, bool *p_open)
{
	DxCamera &camera = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].cameraList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentCameraId];

	static Point position = camera.eye;
	static Point lookat = camera.at;
	static Vector up = camera.up;
	static float cnear = camera.cNear;
	static float cfar = camera.cFar;
	static float aspect = camera.aspect;
	static float cwidth = camera.cWidth;
	static float cheight = camera.cHeight;

	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Camera Change", p_open))
	{
		ImGui::End();
		return;
	}

	if (position == lookat)
	{
		lookat[0] = position[0] + 1.f;
		lookat[1] = position[1] + 1.f;
		lookat[2] = position[2] + 1.f;
	}

	ImGui::InputFloat3("camera position", &position.x);
	ImGui::InputFloat3("camera loook-at", &lookat.x);

	if (up[0] == 0 && up[1] == 0 && up[2] == 0)
	{
		up[1] = 1.f;
	}
	ImGui::InputFloat3("camera up", &up.x);

	if (cfar == cnear)	cfar = cnear + 0.1;
	ImGui::InputFloat("camera near face", &cnear);
	ImGui::InputFloat("camera farface", &cfar);

	if (aspect == 0.f)	aspect = 0.01;
	ImGui::InputFloat("camera visual angle", &aspect);

	if (cwidth == 0.f)	cwidth = 0.01;
	ImGui::InputFloat("camera width", &cwidth);

	if (cheight == 0.f)	cheight = 0.01;
	ImGui::InputFloat("camera height", &cheight);

	if (ImGui::Button("OK", ImVec2(100, 20)))
	{
		camera.eye = position;
		camera.at = lookat;
		camera.up = Normalize(up);
		camera.cNear = cnear;
		camera.cFar = cfar;
		camera.aspect = aspect;
		camera.cWidth = cwidth;
		camera.cHeight = cheight;
		camera.mView = LookAt(camera.eye, camera.at, camera.up).m;
		camera.mProjection = MatrixPerspectiveFov(aspect*3.1415f/360.f, camera.cWidth / camera.cHeight, camera.cNear, camera.cFar).m;
		show_camera_change_view = false;
	}
	ImGui::End();
}

static void DirectionLightSettingView(BasicManager &basicMng, LowLevelRendermanager &renderMng,bool *p_open)
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Direction Light Setting", p_open))
	{
		ImGui::End();
		return;
	}

	static float dir[3] = { 0.707f,-0.707f,0.f };
	static float color[3] = { 1.f,1.f,1.f };
	static float intensity = 1;

	if (dir[0] == 0.f && dir[1] == 0.f && dir[2] == 0.f)	dir[0] = dir[1] = dir[2] = 1.f;
	ImGui::InputFloat3("Light Direction", dir);
	for (int i = 0; i < 3; i++)
	{
		if (color[i] < 0.f)	color[i] = 1.f;
	}
	ImGui::InputFloat3("Light Color", color);
	ImGui::InputFloat("Light Intensity", &intensity);

	if (ImGui::Button("OK", ImVec2(-1, 20)))
	{
		renderMng.lightManager.AddDirLight(basicMng.dxDevice, XMFLOAT4(color[0], color[1], color[2], 1.0), intensity, XMFLOAT4(dir[0], dir[1], dir[2], 0.0));
		show_direction_light_setting_view = false;
	}

	ImGui::End();
}

static void PointLightSettingView(BasicManager &basicMng, bool *p_open)
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Point Light Setting", p_open))
	{
		ImGui::End();
		return;
	}

	static float pos[3] = { 0,0,0 };
	static float color[3] = { 0,0,0 };

	ImGui::InputFloat3("Light Position", pos);
	for (int i = 0; i < 3; i++)
	{
		if (color[i] < 0.f)	color[i] = 1.f;
	}
	ImGui::InputFloat3("Light Color", color);

	if (ImGui::Button("OK", ImVec2(-1, 20)))
	{
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentPlId = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId;
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].plList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId].Pos.x = pos[0];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].plList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId].Pos.y = pos[1];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].plList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId].Pos.z = pos[2];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].plList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId].Color.x = color[0];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].plList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId].Color.y = color[1];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].plList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId].Color.z = color[2];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endPlId++;
		show_point_light_setting_view = false;
	}

	ImGui::End();
}

static void RenderSettingView(WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng, bool *p_open)
{
	RECT rect;
	GetClientRect(winDev.hwnd, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	static wchar_t vsShaderNameWchar[129] = L"";
	static char vsShaderNameChar[128] = "Shader/PBRGeometryPass.fx"; // "hlsl.fx";
	static char vsName[128] = "VS";
	static char vsVersion[128] = "vs_5_0";

	static wchar_t psShaderNameWchar[129] = L"";
	static char psShaderNameChar[128] = "Shader/PBRLightShading.fx"; // "hlsl.fx";
	static char psName[128] = "PS";
	static char psVersion[128] = "ps_5_0";

	ImGui::SetNextWindowPos(ImVec2(w*0.8, 20));
	ImGui::SetNextWindowSize(ImVec2(w*0.2, h*0.77));

	if (!ImGui::Begin("DX11 Render Setting", p_open, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Set render pipeline step by step:");
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Shader Create and Complise"))
	{
		ImGui::Text("Vertex Shader:");
		ImGui::InputText("Vertex Shader file name:", vsShaderNameChar, 128);
		CharToWchar(vsShaderNameChar, vsShaderNameWchar);
		ImGui::InputText("Vertex Shader main program name:", vsName, 128);
		ImGui::InputText("Vertex Shader version:", vsVersion, 128);

		if (ImGui::BeginPopupModal("VS Error1"))
		{
			ImGui::Text("can't load VS shader");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("PS Error1"))
		{
			ImGui::Text("can't load PS shader");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::Button("Vertex Create shader", ImVec2(200, 20)))
		{
			if (!renderMng.shaderManager.LoadAndCreateVertexShader(vsShaderNameWchar, vsName, vsVersion, basicMng.dxDevice))
			{
				ImGui::OpenPopup("VS Error1");
			}
			else if (!renderMng.shaderManager.LoadAndCreatePixelShader(vsShaderNameWchar, psName, psVersion, basicMng.dxDevice))
			{
				ImGui::OpenPopup("PS Error1");
			}
		}

		ImGui::Text("Pixel Shader:");
		ImGui::InputText("Pixel Shader file name:", psShaderNameChar, 128);
		CharToWchar(psShaderNameChar, psShaderNameWchar);
		ImGui::InputText("Pixel Shader main program name:", psName, 128);
		ImGui::InputText("Pixel Shader version:", psVersion, 128);

		if (ImGui::BeginPopupModal("VS Error2"))
		{
			ImGui::Text("can't load VS shader");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("PS Error2"))
		{
			ImGui::Text("can't load PS shader");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::Button("Pixel Create shader", ImVec2(200, 20)))
		{
			if (!renderMng.shaderManager.LoadAndCreateVertexShader(psShaderNameWchar, vsName, vsVersion, basicMng.dxDevice))
			{
				ImGui::OpenPopup("VS Error2");
			}
			else if (!renderMng.shaderManager.LoadAndCreatePixelShader(psShaderNameWchar, psName, psVersion, basicMng.dxDevice))
			{
				ImGui::OpenPopup("PS Error2");
			}
		}
	}


	if (ImGui::CollapsingHeader("Scene primitive input"))
	{


		if (basicMng.sceneManager.sceneList[RenderSceneId].endUnityId <= 0)
		{
			ImGui::Text(" Scene %d don't have unity!", RenderSceneId);
		}
		else
		{
			ImGui::Text("Scene has %d unity!", basicMng.sceneManager.sceneList[RenderSceneId].endUnityId);
		}
		ImGui::Separator();


		static int sceneid1 = 0;
		ImGui::Text("Scene ID:");
		ImGui::SameLine();
		if (sceneid1 < 0 || sceneid1>23)	sceneid1 = 0;
		ImGui::InputInt("Scene ID", &sceneid1);
		if (ImGui::Button("Input Scene"))
		{
			RenderSceneId = sceneid1;
			basicMng.sceneManager.currentSceneId = sceneid1;
		}

		ImGui::Text("Current Scene %d", basicMng.sceneManager.currentSceneId);
		if (ImGui::Button("Input Current Scene"))
		{
			RenderSceneId = basicMng.sceneManager.currentSceneId;
		}


	}

	if (ImGui::CollapsingHeader("Camera input"))
	{
		static int cameraId = 0;
		if (basicMng.sceneManager.sceneList[RenderSceneId].endCameraId <= 0)
		{
			ImGui::Text("Scene %d don't set any camera!", RenderSceneId);

		}
		else
		{
			ImGui::Text("Scene %d has %d camera!", RenderSceneId, basicMng.sceneManager.sceneList[RenderSceneId].endCameraId);
			ImGui::Separator();

			ImGui::Text("which camera you choose:");
			if (cameraId<0 || cameraId>basicMng.sceneManager.sceneList[RenderSceneId].maxCameraNum - 1)
			{
				cameraId = 0;
			}
			ImGui::InputInt("Camera ID", &cameraId);
			/*
			if (ImGui::Button("Create Camera"))
			{
				RenderCameraId = cameraId;
				basicMng.sceneManager.sceneList[RenderSceneId].currentCameraId = cameraId;
				renderMng.cameraManager.CreateViewBuffer(basicMng.dxDevice);
				renderMng.cameraManager.CreateProjectionBuffer(basicMng.dxDevice);
			}
			*/
			ImGui::Text("Current Camera %d", basicMng.sceneManager.sceneList[RenderSceneId].currentCameraId);
			if (ImGui::Button("Create Current Camera"))
			{
				RenderCameraId = basicMng.sceneManager.sceneList[RenderSceneId].currentCameraId;
				renderMng.cameraManager.CreateWorldBuffer(basicMng.dxDevice);
				renderMng.cameraManager.CreateViewBuffer(basicMng.dxDevice);
				renderMng.cameraManager.CreateProjectionBuffer(basicMng.dxDevice);
			}
		}


	}

	/*
	if (ImGui::CollapsingHeader("Light input"))
	{

		if (basicMng.sceneManager.sceneList[RenderSceneId].endDlId <= 0)
		{
			ImGui::Text("Scene %d don't set any direction light!", RenderSceneId);
		}
		else
		{
			if (ImGui::Button("Create Direction Light"))
			{
				renderMng.lightManager.CreateBuffer(basicMng.dxDevice, basicMng.sceneManager.sceneList[RenderSceneId].dlList[0]);
			}
		}

		ImGui::Separator();
		if (basicMng.sceneManager.sceneList[RenderSceneId].endPlId <= 0)
		{
			ImGui::Text("Scene %d don't set any point light!", RenderSceneId);
		}
		else
		{
			if (ImGui::Button("Create Point Light"))
			{
				renderMng.lightManager.CreateBuffer(basicMng.dxDevice, basicMng.sceneManager.sceneList[RenderSceneId].plList[0]);
			}
		}
	}
	*/

	//vewiport 
	static float vppos[2] = { 0.f,0.f };
	float w1 = GetSystemMetrics(SM_CXSCREEN);
	float h2 = GetSystemMetrics(SM_CYSCREEN);
	static float vpwh[2] = { w1,h2 };
	static float vpdepth[2] = { 0.f,1.f };

	if (ImGui::CollapsingHeader("set viewport"))
	{
		for (int i = 0; i < 2; i++)
		{
			if (vppos[i] < 0.f)	vppos[i] = 0.f;
			if (vpwh[i] <= 0.f)	vpwh[i] = 0.f;
		}
		ImGui::InputFloat2("viewport position:", vppos);
		ImGui::InputFloat2("viewport width and height:", vpwh);
		ImGui::InputFloat2("viewport min and max depth:", vpdepth);

		if (ImGui::Button("Set ViewPort"))
		{
			renderMng.SetViewPort(basicMng, vppos[0], vppos[1], vpwh[0], vpwh[1], vpdepth[0], vpdepth[1]);
		}
	}

	ImGui::End();
}

static void MaterialChangeView(BasicManager &basicMng, LowLevelRendermanager& renderMng, bool *p_open)
{	

}

static void SceneHierarchy(bool* p_open, BasicManager &basicMng)
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Scene Hierarchy", p_open))
	{
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
	ImGui::Columns(2);
	ImGui::Separator();

	struct funcs
	{
		static void ShowDummyObject(Unity &unity, int uid)
		{
			ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
			ImGui::AlignFirstTextHeightToWidgets();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
			bool node_open = ImGui::TreeNode("Unity", "%s_%u", unity.name, uid);
			ImGui::NextColumn();
			ImGui::AlignFirstTextHeightToWidgets();
			if(ImGui::Button("Change", ImVec2(100, 15)))
			{
				Show_Unity_Property = true;
				SelectUnityID = unity.UnityId;
			}
			ImGui::NextColumn();
			if (node_open )
			{   
				if(unity.childCount >0)
				{
					for (int i = 0; i < unity.childCount; i++)
					{
						ImGui::PushID(i); // Use field index as identifier.
						ShowDummyObject(*unity.childs[i], 424242);
						ImGui::PopID();
					}					
				}

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	};

	// Iterate dummy objects with dummy members (all the same data)
	DxScene scene = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId];
	int unityCount = scene.endUnityId - 0;
	if (unityCount > 0)
	{
		for (int obj_i = 0; obj_i < unityCount; obj_i++)
			if(scene.unityList[obj_i].parent == NULL && scene.unityList[obj_i].UnityId >=0)
				funcs::ShowDummyObject(scene.unityList[obj_i], obj_i);
	}


	ImGui::Columns(1);
	ImGui::Separator();
	ImGui::PopStyleVar();
	ImGui::End();
}

static void ShowTextureResource(bool* p_open, BasicManager &basicMng)
{
	ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiSetCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Show Texture Resource", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	//for (int i = 0; i < basicMng.textureManager.endTextureId; i++)
	//{	
	//	if (basicMng.textureManager.dxTexure2D != NULL)
	//	{
	//		ImGui::PushID(i);
	//		int frame_padding = -1 + i;     // -1 = uses default padding
	//		ImGui::Button(basicMng.textureManager.texNames[i], ImVec2(100, 20));
	//		ImGui::PopID();
	//	}
	//	ImGui::SameLine();
	//}

	// left
	static int selected = 0;
	ImGui::BeginChild("left pane", ImVec2(150, 0), true);
	for (int i = 0; i < basicMng.textureManager.endTextureId; i++)
	{
		char label[128];
		sprintf_s(label, basicMng.textureManager.texNames[i]);
		if (ImGui::Selectable(label, selected == i))
			selected = i;
	}
	ImGui::EndChild();


	//add texture button
	static wstring texturePath;
	static string error = "no error!";
	static char str1[128] = "Texture/Default_albedo.png";
	ImGui::InputText("Texture Name", str1, 128);
	texturePath = CharToWstring(str1);

	ImGui::PushItemWidth(120);
	static int type = 1;
	ImGui::Combo("sRGB", &type, "No\0Yes\0\0");
	static bool sRGB = true;
	if (type == 0)	sRGB = false;
	if (type == 1)	sRGB = true;

	if (ImGui::BeginPopupModal("texture error"))
	{
		ImGui::Text("can't load this texture!");
		if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	if (ImGui::Button("Add Texture", ImVec2(-1, 20)))
	{
		bool isLoaded = basicMng.textureManager.DxLoadImage(str1, basicMng.dxDevice, sRGB);
		if (!isLoaded)
		{
			ImGui::OpenPopup("texture error");
		}
	}

	ImGui::End();
}

void ShowObjectResource(bool * p_open, BasicManager & basicMng)
{
	ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiSetCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Show Object Resource", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	for (int i = 0; i < basicMng.objManager.endObjId; i++)
	{
		if (basicMng.objManager.DxObjMem[i]->empty != true)
		{
			ImGui::PushID(i);
			int frame_padding = -1 + i;     // -1 = uses default padding
			const char name = (char)("0" + i);
			ImGui::Button(&name, ImVec2(100, 20));
			ImGui::PopID();
		}
		//ImGui::SameLine();
	}
	ImGui::End();
}

void ShowMaterialResource(bool * p_open, BasicManager & basicMng)
{
	ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiSetCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Show Material Resource", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	for (int i = 0; i < basicMng.materialsManager.endMtlId; i++)
	{
		if (basicMng.materialsManager.dxMaterial[i].empty != true)
		{
			ImGui::PushID(i);
			int frame_padding = -1 + i;     // -1 = uses default padding
			const char name = (char)("0" + i);
			ImGui::Button(&name, ImVec2(100, 20));
			ImGui::PopID();
		}
		//ImGui::SameLine();
	}
	ImGui::End();
}

void ShowLightResource(bool * p_open, BasicManager & basicMng)
{
	ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiSetCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Show Material Resource", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();
}

void ShowUnityProperty(bool* p_open, WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng, int SelectUnityID)
{
	RECT rect;
	GetClientRect(winDev.hwnd, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);

	DxScene& scene = basicMng.sceneManager.sceneList[0];

	ImGui::SetNextWindowPos(ImVec2(w*0.6, 20));
	ImGui::SetNextWindowSize(ImVec2(w*0.2, h*0.77));

	if (!ImGui::Begin("Unity Property Editor", p_open, ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		ImGui::End();
		return;
	}

	if (SelectUnityID <0 || SelectUnityID >= scene.endUnityId)
	{	
		ImGui::Text("No Unity Info");
		ImGui::Text("Unity ID:  %.0f", SelectUnityID);
		ImGui::End();
		return;
	}
	
	Unity& unity = scene.unityList[SelectUnityID];
	DxObj* obj = basicMng.objManager.DxObjMem[unity.objId];

	if (ImGui::CollapsingHeader("Wolrd Transform info"))
	{	
		ImGui::InputFloat("X", &unity.wolrdTransform.m.m._11);
		ImGui::SameLine();
		ImGui::InputFloat("Y", &unity.wolrdTransform.m.m._12);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._13);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._14);

		ImGui::InputFloat("X", &unity.wolrdTransform.m.m._21);
		ImGui::SameLine();
		ImGui::InputFloat("Y", &unity.wolrdTransform.m.m._22);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._23);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._24);

		ImGui::InputFloat("X", &unity.wolrdTransform.m.m._31);
		ImGui::SameLine();
		ImGui::InputFloat("Y", &unity.wolrdTransform.m.m._32);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._33);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._34);

		ImGui::InputFloat("X", &unity.wolrdTransform.m.m._41);
		ImGui::SameLine();
		ImGui::InputFloat("Y", &unity.wolrdTransform.m.m._42);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._43);
		ImGui::SameLine();
		ImGui::InputFloat("Z", &unity.wolrdTransform.m.m._44);
	}

	if (ImGui::CollapsingHeader("Object Info"))
	{
		if(unity.objId <0 || unity.objId >= basicMng.objManager.endObjId)
		{
			ImGui::Text("No Object Info");
			ImGui::Text("Object ID:  %.0f", unity.objId);
		}
		else
		{
			ImGui::Text("Object ID: %d", unity.objId);
			ImGui::Text("Vertex count: %d", obj->vertexNum);
			ImGui::Text("Face count: %d", obj->faceNum);
			ImGui::Text("Maertial count: %d",obj->materialNum);
		}

	}

	if (ImGui::CollapsingHeader("Material"))
	{
		if (unity.materialNum <= 0 || unity.MaterialsIdIndex == NULL)
		{
			ImGui::Text("No Material Info");
		}
		else
		{
			for (int i = 0; i < unity.materialNum; i++)
			{	
				const char* name = "0" + i;
				if (ImGui::CollapsingHeader(name))
				{	
					int matID = unity.MaterialsIdIndex[i];
					if(matID < 0 || matID >= basicMng.materialsManager.endMtlId)
					{	
						ImGui::Text("No Material Info");
						ImGui::Text("Material ID: %d", matID);
					}
					else
					{
						DxMaterials& mat = basicMng.materialsManager.dxMaterial[matID];
						ImGui::Text("Material ID: %d", matID);

						//color
						ImGui::ColorEdit4("Color", (float*)&mat.parameter._ColorFactor);

						//albedo
						ImGui::Text("Albedo Tex ID: %d", mat.albedoID);

						if (ImGui::BeginPopupModal("albedo editor"))
						{
							ImGui::Text("choose a texture!");

							static int selected = 0;
							ImGui::BeginChild("left pane", ImVec2(0, 0), true);
							for (int ind = 0; ind < basicMng.textureManager.endTextureId; ind++)
							{
								char label[128];
								string texN = "%d ,";
								texN += basicMng.textureManager.texNames[ind];
								sprintf_s(label, texN.c_str(), ind);
								if (ImGui::Selectable(label, selected == ind))
									selected = ind;
							}
							ImGui::EndChild();

							if (ImGui::Button("OK"))
							{
								mat.albedoID = selected;
								mat.albedoSampleState = &basicMng.textureManager.sampleStatePointer[selected];
								mat.albedoSRV = &basicMng.textureManager.texViewPointer[selected];
								ImGui::CloseCurrentPopup();
							}
							if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}

						if (ImGui::Button("Change Texture", ImVec2(150, 20)))
						{
								ImGui::OpenPopup("albedo editor");
						}
						
						//normal
						ImGui::Text("normal Tex ID: %d", mat.normalID);

						if (ImGui::BeginPopupModal("normal editor"))
						{
							ImGui::Text("choose a texture!");

							static int selected = 0;
							ImGui::BeginChild("left pane", ImVec2(0, 0), true);
							for (int ind = 0; ind < basicMng.textureManager.endTextureId; ind++)
							{
								char label[128];
								string texN = "%d ,";
								texN += basicMng.textureManager.texNames[ind];
								sprintf_s(label, texN.c_str(), ind);
								if (ImGui::Selectable(label, selected == ind))
									selected = ind;
							}
							ImGui::EndChild();

							if (ImGui::Button("OK"))
							{
								mat.normalID = selected;
								mat.normalSampleState = &basicMng.textureManager.sampleStatePointer[selected];
								mat.normalSRV = &basicMng.textureManager.texViewPointer[selected];
								ImGui::CloseCurrentPopup();
							}
							if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}

						if (ImGui::Button("Change normal", ImVec2(150, 20)))
						{
							ImGui::OpenPopup("normal editor");
						}

						//mra
						ImGui::Text("Metallic, Roughness and AO Tex ID: %d", mat.mraID);

						if (ImGui::BeginPopupModal("mra editor"))
						{
							ImGui::Text("choose a texture!");

							static int selected = 0;
							ImGui::BeginChild("left pane", ImVec2(0, 0), true);
							for (int ind = 0; ind < basicMng.textureManager.endTextureId; ind++)
							{
								char label[128];
								string texN = "%d ,";
								texN += basicMng.textureManager.texNames[ind];
								sprintf_s(label, texN.c_str(), ind);
								if (ImGui::Selectable(label, selected == ind))
									selected = ind;
							}
							ImGui::EndChild();

							if (ImGui::Button("OK"))
							{
								mat.mraID = selected;
								mat.mraSampleState = &basicMng.textureManager.sampleStatePointer[selected];
								mat.mraSRV = &basicMng.textureManager.texViewPointer[selected];
								ImGui::CloseCurrentPopup();
							}
							if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
							ImGui::EndPopup();
						}
						
						if (ImGui::Button("Change mra", ImVec2(150, 20)))
						{
							ImGui::OpenPopup("mra editor");
						}
	
						//parametr
						ImGui::SliderFloat("Metallic Fatcor",&mat.parameter._MetallicFactor, 0, 1, "%.2f");
						ImGui::SliderFloat("Roughness Fatcor", &mat.parameter._RoughnessFactor, 0, 1, "%.2f");
						ImGui::SliderFloat("IBL Fatcor", &mat.parameter._IBLFactor, 0, 1, "%.2f");

						//tilling and scaling
						ImGui::SliderFloat("scaleX", &mat.parameter.scaleX, 0, 10, "%.2f");
						ImGui::SliderFloat("scaleY", &mat.parameter.scaleY, 0, 10, "%.2f");
						ImGui::SliderFloat("offsetX", &mat.parameter.offsetX, -10, 10, "%.2f");
						ImGui::SliderFloat("offsetY", &mat.parameter.offsetY, -10, 10, "%.2f");

						//disney lerp para
						ImGui::SliderFloat("eta", &mat.lerp.eta, 1, 10, "%.2f");
						ImGui::SliderFloat("strans", &mat.lerp.strans, 0, 1, "%.2f");
						ImGui::SliderFloat("dt", &mat.lerp.dt, 0, 1, "%.2f");
						ImGui::SliderFloat("sheenWeight", &mat.lerp.sheenWeight, 0, 1, "%.2f");
						ImGui::SliderFloat("sTint", &mat.lerp.sTint, 0, 1, "%.2f");
						ImGui::SliderFloat("flat", &mat.lerp.flat, 0, 1, "%.2f");
						ImGui::SliderFloat("anisotropic", &mat.lerp.anisotropic, 0, 1, "%.2f");
						ImGui::SliderFloat("specTint", &mat.lerp.specTint, 0, 1, "%.2f");
						ImGui::SliderFloat("cc", &mat.lerp.cc, 0, 1, "%.2f");
						ImGui::SliderFloat("glossTint", &mat.lerp.glossTint, 0, 1, "%.2f");

						//ambient color
						ImGui::ColorEdit4("Ambient Color", (float*)&mat.parameter._AmbientClor);
						
					}
				}
			}
		}
	}

	ImGui::End();
}

void ShowRayTracer(bool* p_open, BasicManager &basicMng, LowLevelRendermanager &renderMng, GradientPathTracer& optixGptMng)
{
	ImGui::SetNextWindowPos(ImVec2(20, 600), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(800, 300), ImGuiSetCond_FirstUseEver);

	// Main body of the Demo window starts here.
	if (!ImGui::Begin("Show RayTracer", p_open))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::InputInt("Sample Count", &optixGptMng.sqrtNumSamples);
	ImGui::InputInt("RR Depth", &optixGptMng.rrBeginDepth);
	ImGui::InputInt("Max Depth", &optixGptMng.loopCount);
	ImGui::InputFloat("Scene Epsilon", &optixGptMng.sceneEpsilon);

	ImGui::PushItemWidth(120);
	static int type = 0;
	ImGui::Combo("NGPT Denoise Type", &type, "No\0Gradients And E-LPIPS Loss\0E-LPIPS Loss\0Gradients And L1 Loss\0L1 Loss\0\0");
	if (type == 0)	optixGptMng.denoiseType = NgptDenoiseType::None;
	if (type == 1)	optixGptMng.denoiseType = NgptDenoiseType::GraAndELoss;
	if (type == 2)	optixGptMng.denoiseType = NgptDenoiseType::ELoss;
	if (type == 3)	optixGptMng.denoiseType = NgptDenoiseType::GraAndL1Loss;
	if (type == 4)	optixGptMng.denoiseType = NgptDenoiseType::L1Loss;

	if (ImGui::Button("Bake", ImVec2(-1, 20)))
	{
		
		optixGptMng.Bake(basicMng, renderMng);
	}

	ImGui::End();
}