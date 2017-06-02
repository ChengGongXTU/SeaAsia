#include"mainwindowui.h"

static bool show_scene_setting_view = false;
static bool show_scene_resource_view = true;
static bool show_scene_list_view = true;
static bool show_unity_load_view = false;
static bool show_camera_setting_view = false;
static bool show_point_light_setting_view = false;
static bool show_direction_light_setting_view = false;
static bool show_render_setting_view = true;
static bool show_camera_change_view = false;
static bool show_materila_change_view = false;

void MainWindowUI(WindowsDevice & winDev,BasicManager &basicMng, LowLevelRendermanager &renderMng, RayTraceManager& rayMng,bool *p_open)
{	

	if (show_scene_resource_view)	ScenenResourceView(winDev,basicMng,renderMng,&show_scene_resource_view);
	if (show_scene_list_view)	ScenenListView(winDev, basicMng, &show_scene_list_view);
	if (show_render_setting_view) RenderSettingView(winDev, basicMng, renderMng, &show_render_setting_view);
	


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
			if(ImGui::BeginMenu("Scene Resource View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &show_scene_resource_view));
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Key Frame View"))
			{
				if (ImGui::MenuItem("Show the View", NULL, &show_scene_list_view));
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		//Resource manager menu
		if(ImGui::BeginMenu("Resource Manager")){ ImGui::EndMenu(); }

		//Low-Level pipeline Render 
		if (ImGui::BeginMenu("Low-Level Render "))
		{	
			if (ImGui::MenuItem("Open Dx11 Render Setting", NULL, &show_render_setting_view));
			ImGui::EndMenu(); 
		}

		//ray trace render
		if (ImGui::BeginMenu("Ray Trace Render "))
		{	
			if (ImGui::Button("Begin Global Illumination Render"))
			{
				float dsadas;
				rayMng.SetRayTrace(0);
				rayMng.SceneLoad(basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId], basicMng);
				rayMng.Render(dsadas);
			}
			ImGui::EndMenu(); 
		}

		ImGui::EndMainMenuBar();
	}

	if (renderMng.cameraManager.viewTransformBuffer != NULL)
	{	
		DxCamera &camera = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].cameraList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentCameraId];
		RECT rect;
		GetClientRect(winDev.hwnd, &rect);
		float rw = (float)(rect.right - rect.left);
		float rh = (float)(rect.bottom - rect.top);
		ImGui::SetNextWindowPos(ImVec2(rw*0.7, 20));
		ImGui::SetNextWindowSize(ImVec2(rw*0.1, rh*0.15));
		if (!ImGui::Begin(""))
		{
			ImGui::End();
			return;
		}

		if (ImGui::Button("front"))
		{
			//XMFLOAT3  eyeVec = XMFLOAT3(XMVectorGetX(camera.at) - XMVectorGetX(camera.eye),
			//	XMVectorGetY(camera.at) - XMVectorGetY(camera.eye),
			//	XMVectorGetZ(camera.at) - XMVectorGetZ(camera.eye));

			//camera.eye = XMVectorSet(XMVectorGetX(camera.eye) + eyeVec.x,
			//	XMVectorGetY(camera.eye) + eyeVec.y,
			//	XMVectorGetZ(camera.eye) + eyeVec.z,1.f);
			//camera.mView = XMMatrixLookAtLH(camera.eye, camera.at, camera.up);
		}

		if (ImGui::Button("front"))
		{
			//XMFLOAT3  eyeVec = XMFLOAT3(XMVectorGetX(camera.at) - XMVectorGetX(camera.eye),
			//	XMVectorGetY(camera.at) - XMVectorGetY(camera.eye),
			//	XMVectorGetZ(camera.at) - XMVectorGetZ(camera.eye));

			//camera.eye = XMVectorSet(XMVectorGetX(camera.eye) + eyeVec.x,
			//	XMVectorGetY(camera.eye) + eyeVec.y,
			//	XMVectorGetZ(camera.eye) + eyeVec.z, 1.f);
			//camera.mView = XMMatrixLookAtLH(camera.eye, camera.at, camera.up);
		}


		ImGui::End();
	}

}

static void ScenenResourceView(WindowsDevice & winDev, BasicManager &basicMng, LowLevelRendermanager &renderMng, bool *p_open)
{
	
	if (show_scene_setting_view) SceneSettingView(basicMng, &show_scene_setting_view);
	if (show_unity_load_view)	UnityLoadingView(basicMng, renderMng,&show_unity_load_view);
	if (show_camera_setting_view) CameraSettingView(basicMng, &show_camera_setting_view);
	if (show_direction_light_setting_view) DirectionLightSettingView(basicMng, &show_direction_light_setting_view);
	if (show_point_light_setting_view)	PointLightSettingView(basicMng, &show_point_light_setting_view);
	if (show_camera_change_view)	CameraCangeView(basicMng, &show_camera_change_view);
	if (show_materila_change_view)	MaterialChangeView(basicMng, renderMng,&show_materila_change_view);

	RECT rect;
	GetClientRect(winDev.hwnd, &rect);
	float w = (float)(rect.right - rect.left);
	float h = (float)(rect.bottom - rect.top);
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::SetNextWindowSize(ImVec2(w*0.15, h*0.77));
	/*ImGui::SetNextWindowSizeConstraints(ImVec2(-1, -1), ImVec2(-1, -1));*/

	if (!ImGui::Begin("Scene Resource View", p_open,ImGuiWindowFlags_AlwaysHorizontalScrollbar| ImGuiWindowFlags_AlwaysVerticalScrollbar))
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
						ImGui::Text("Texture ID: %d", unity.textureId);
						ImGui::Text("Position in world: %f %f %f", unity.Pos.x, unity.Pos.y, unity.Pos.z);

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

						ImGui::Text("Camera position : %f %f %f", XMVectorGetX(camera.eye), XMVectorGetY(camera.eye), XMVectorGetZ(camera.eye));
						ImGui::Text("Camera look at : %f %f %f", XMVectorGetX(camera.at), XMVectorGetY(camera.at), XMVectorGetZ(camera.at));
						ImGui::Text("Camera up : %f %f %f", XMVectorGetX(camera.up), XMVectorGetY(camera.up), XMVectorGetZ(camera.up));
						ImGui::Text("Camera near face: %f", camera.cNear);
						ImGui::Text("Camera far face: %f", camera.cFar);
						ImGui::Text("Camera view angle: %f",camera.aspect);
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
		&&basicMng.sceneManager.sceneList[i].cameraList == NULL
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

static void ScenenListView(WindowsDevice & winDev,BasicManager &basicMng, bool *p_open)
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

	float tex_w = (float)ImGui::GetIO().Fonts->TexWidth;
	float tex_h = (float)ImGui::GetIO().Fonts->TexHeight;
	ImTextureID tex_id = ImGui::GetIO().Fonts->TexID;

	for (int i = 0; i < basicMng.sceneManager.endSceneId; i++)
	{
		ImGui::PushID(i);
		if (ImGui::ImageButton(tex_id, ImVec2(80, 80), ImVec2(0, 0), ImVec2(1, 1), 3, ImColor(0, 0, 0, 255)))
		{
			basicMng.sceneManager.currentSceneId = i;
		}
		ImGui::PopID();
		ImGui::SameLine();
	}
	ImGui::End();
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
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].setUp(500, 10, 10, 10);
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

		static char str1[128] = "1.obj";
		static char str2[128] = "1.mtl";
		static char str3[128] = "1.bmp";


		ImGui::InputText("Input OBJ File Name",str1, 128);
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
				basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId], basicMng,otype))
			{	
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

			else if ( textureName[0] !=NULL && basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].unityList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endUnityId].textureId == -1)
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

	ImGui::End();
}

static void CameraSettingView(BasicManager &basicMng, bool *p_open)
{	

	static float position[3] = { 0,0,0 };
	static float lookat[3] = { 0,0,1.f };
	static float up[3] = { 0,1.f,0 };
	static float cnear = 0.1f;
	static float cfar = 10000.f;
	static float aspect = M_PI / 4;
	static float cwidth = 1280.f;
	static float cheight = 720.f;

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

	if (up[0] ==0 && up[1] ==0&& up[2] == 0)
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
			position[0], position[1], position[2], lookat[0], lookat[1], lookat[2], up[0], up[1], up[2], cnear, cfar, aspect, cwidth, cheight);
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endCameraId++;
		show_camera_setting_view = false;
	}


	ImGui::End();

}

static void CameraCangeView(BasicManager &basicMng, bool *p_open)
{	
	DxCamera &camera = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].cameraList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentCameraId];
	
	static float position[3] = { XMVectorGetX(camera.eye), XMVectorGetY(camera.eye), XMVectorGetZ(camera.eye) };
	static float lookat[3] = { XMVectorGetX(camera.at), XMVectorGetY(camera.at), XMVectorGetZ(camera.at) };
	static float up[3] = { XMVectorGetX(camera.up), XMVectorGetY(camera.up), XMVectorGetZ(camera.up) };
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

	if (ImGui::Button("OK", ImVec2(100, 20)))
	{
		camera.eye = XMVectorSet(position[0], position[1], position[2],1.f);
		camera.at = XMVectorSet(lookat[0], lookat[1], lookat[2], 1.f);
		camera.up = XMVectorSet(up[0], up[1], up[2], 0.f);
		camera.cNear = cnear;
		camera.cFar = cfar;
		camera.aspect = aspect;
		camera.cWidth = cwidth;
		camera.cHeight = cheight;
		camera.mView = XMMatrixLookAtLH(camera.eye, camera.at,camera.up);
		camera.mProjection = XMMatrixPerspectiveFovLH(aspect, camera.cWidth / camera.cHeight, camera.cNear, camera.cFar);
		show_camera_change_view = false;
	}
	ImGui::End();
}

static void DirectionLightSettingView(BasicManager &basicMng, bool *p_open)
{
	ImGui::SetNextWindowSize(ImVec2(430, 450), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Direction Light Setting", p_open))
	{
		ImGui::End();
		return;
	}

	static float dir[3] = { 5.f,5.f,5.f };
	static float color[3] = { 1.f,1.f,1.f };

	if (dir[0] ==0.f && dir[1] ==0.f && dir[2] == 0.f)	dir[0] = dir[1] = dir[2] = 1.f;
	ImGui::InputFloat3("Light Direction", dir);
	for (int i = 0; i < 3; i++)
	{
		if (color[i] < 0.f)	color[i] = 1.f;
	}
	ImGui::InputFloat3("Light Color", color);

	if (ImGui::Button("OK", ImVec2(-1, 20)))
	{
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentDlId = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId;
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].dlList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId].Dir.x = dir[0];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].dlList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId].Dir.y = dir[1];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].dlList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId].Dir.z = dir[2];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].dlList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId].Color.x = color[0];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].dlList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId].Color.y = color[1];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].dlList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId].Color.z = color[2];
		basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].endDlId++;
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
	static char vsShaderNameChar[128] = "hlsl.fx";
	static char vsName[128] = "VS";
	static char vsVersion[128] = "vs_4_0";

	static wchar_t psShaderNameWchar[129] = L"";
	static char psShaderNameChar[128] = "hlsl.fx";
	static char psName[128] = "PS";
	static char psVersion[128] = "ps_4_0";

	ImGui::SetNextWindowPos(ImVec2(w*0.8, 20));
	ImGui::SetNextWindowSize(ImVec2(w*0.2, h*0.77));

	if (!ImGui::Begin("DX11 Render Setting", p_open,ImGuiWindowFlags_AlwaysHorizontalScrollbar|ImGuiWindowFlags_AlwaysVerticalScrollbar))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Set render pipeline step by step:");
	ImGui::Separator();

	if (ImGui::CollapsingHeader("Shader Create and Complise"))
	{	
		ImGui::Text("Vertex Shader:");
		ImGui::InputText("Vertex Shader file name:", vsShaderNameChar,128);
		CharToWchar(vsShaderNameChar,vsShaderNameWchar);
		ImGui::InputText("Vertex Shader main program name:", vsName, 128);
		ImGui::InputText("Vertex Shader version:", vsVersion, 128);

		if (ImGui::BeginPopupModal("VS Error"))
		{
			ImGui::Text("can't load VS shader");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("PS Error"))
		{
			ImGui::Text("can't load PS shader");
			if (ImGui::Button("close"))	ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		if (ImGui::Button("Vertex Create shader",ImVec2(200,20)))
		{
			if (!renderMng.shaderManager.LoadAndCreateVertexShader(vsShaderNameWchar, vsName, vsVersion, basicMng.dxDevice))
			{
				ImGui::OpenPopup("VS Error");
			}
		}

		ImGui::Text("Pixel Shader:");
		ImGui::InputText("Pixel Shader file name:", psShaderNameChar, 128);
		CharToWchar(psShaderNameChar,psShaderNameWchar);
		ImGui::InputText("Pixel Shader main program name:", psName, 128);
		ImGui::InputText("Pixel Shader version:", psVersion, 128);

		if (ImGui::Button("Pixel Create shader", ImVec2(200, 20)))
		{
			if (!renderMng.shaderManager.LoadAndCreatePixelShader(psShaderNameWchar, psName, psVersion, basicMng.dxDevice))
			{
				ImGui::OpenPopup("PS Error");
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
		if(ImGui::Button("Input Scene"))
		{
			RenderSceneId = sceneid1;
			basicMng.sceneManager.currentSceneId = sceneid1;
		}

		ImGui::Text("Current Scene %d",basicMng.sceneManager.currentSceneId);
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
			if (ImGui::Button("Create Camera"))
			{
				RenderCameraId = cameraId;
				basicMng.sceneManager.sceneList[RenderSceneId].currentCameraId = cameraId;
				renderMng.cameraManager.CreateViewBuffer(basicMng.dxDevice);
				renderMng.cameraManager.CreateProjectionBuffer(basicMng.dxDevice);
			}

			ImGui::Text("Current Camera %d", basicMng.sceneManager.sceneList[RenderSceneId].currentCameraId);
			if (ImGui::Button("Create Current Camera"))
			{
				RenderCameraId = basicMng.sceneManager.sceneList[RenderSceneId].currentCameraId;
				renderMng.cameraManager.CreateViewBuffer(basicMng.dxDevice);
				renderMng.cameraManager.CreateProjectionBuffer(basicMng.dxDevice);
			}		
		}


	}

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

	//vewiport 
	static float vppos[2] = { 0.f,0.f };
	static float vpwh[2] = { 1280.f,720.f };
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

static void MaterialChangeView(BasicManager &basicMng,LowLevelRendermanager& renderMng, bool *p_open)
{
	Unity& unity = basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].unityList[basicMng.sceneManager.sceneList[basicMng.sceneManager.currentSceneId].currentUnityId];
	MaterialParameter &currentPara = basicMng.materialsManager.dxMaterial[basicMng.materialsManager.currentMtlId].parameter;

	static float ka[3] = { 0,0,0 };
	static float kd[3] = { 0,0,0 };
	static float ks[3] = { 0,0,0 };
	static float ke[3] = { 0,0,0 };
	static float alpha = 1.f;
	static float illum = 1.f;
	static float Ni = 0.f;
	static int Ns = 1;

	ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Materila Parameter", p_open, ImGuiWindowFlags_MenuBar))
	{
		ImGui::End();
	}

	// left
	ImGui::BeginChild("maeterila number", ImVec2(150, 0), true);
	for (int i = 0; i < unity.materialNum; i++)
	{
		char label[128];
		sprintf_s(label, "Material %d", i);
		if (ImGui::Selectable(label))
			basicMng.materialsManager.currentMtlId = unity.MaterialsIdIndex[i];
	}
	ImGui::EndChild();
	ImGui::SameLine();

	// right
	ImGui::BeginGroup();

	ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing())); // Leave room for 1 line below us
	ImGui::Text("material: %d", basicMng.materialsManager.currentMtlId);
	ImGui::Separator();

	ImGui::Text("Current Materila Ka: %f %f %f", currentPara.Ka.x, currentPara.Ka.y, currentPara.Ka.z);
	ImGui::InputFloat3("Ka:", ka);
	ImGui::Text("Current Materila Kd: %f %f %f", currentPara.Kd.x, currentPara.Kd.y, currentPara.Kd.z);
	ImGui::InputFloat3("Kd:", kd);
	ImGui::Text("Current Materila Ks: %f %f %f", currentPara.Ks.x, currentPara.Ks.y, currentPara.Ks.z);
	ImGui::InputFloat3("Ks:", ks);
	ImGui::Text("Current Materila Ke: %f %f %f", currentPara.Ke.x, currentPara.Ke.y, currentPara.Ke.z);
	ImGui::InputFloat3("Ke:", ke);

	ImGui::Text("Current Materila alpha: %f",currentPara.alpha );
	if (alpha < 0.f)	alpha = 0.f;
	ImGui::InputFloat("alpha:", &alpha);

	ImGui::Text("Current Materila Ni: %f", currentPara.Ni);
	if (Ni < 0.f)	Ni = 0.f;
	ImGui::InputFloat("Ni:", &Ni);

	ImGui::Text("Current Materila illum: %f", currentPara.illum);
	if (illum < 0.f)illum = 0.f;
	ImGui::InputFloat("illum:", &illum);

	ImGui::Text("Current Materila Ns: %d", currentPara.Ns);
	if (Ns < 1)	Ns = 1;
	ImGui::InputInt("Ns:", &Ns);
	ImGui::EndChild();

	static int mtlType = 0;
	ImGui::Combo("Object type", &mtlType, "matte\0phong\0emissive\0\0");

	ImGui::BeginChild("buttons");

	if (ImGui::Button("Set Defeat"))
	{
		ka[0] = currentPara.Ka.x; ka[1] = currentPara.Ka.y; ka[2] = currentPara.Ka.z;
		kd[0] = currentPara.Kd.x; kd[1] = currentPara.Kd.y; kd[2] = currentPara.Kd.z;
		ks[0] = currentPara.Ks.x; ks[1] = currentPara.Ks.y; ks[2] = currentPara.Ks.z;
		ke[0] = currentPara.Ke.x; ke[1] = currentPara.Ke.y; ke[2] = currentPara.Ke.z;
		alpha = currentPara.alpha;
		illum = currentPara.illum;
		Ni = currentPara.Ni;
		Ns = currentPara.Ns;
		mtlType = basicMng.materialsManager.dxMaterial[basicMng.materialsManager.currentMtlId].mtlType;
	}
	ImGui::SameLine();
	if (ImGui::Button("OK"))
	{
		MaterialParameter para;
		para.Ka = XMFLOAT4(ka[0], ka[1], ka[2], 1.f);
		para.Kd = XMFLOAT4(kd[0], kd[1], kd[2], 1.f);
		para.Ks = XMFLOAT4(ks[0], ks[1], ks[2], 1.f);
		para.Ke = XMFLOAT4(ke[0], ke[1], ke[2], 1.f);
		para.alpha = alpha;
		para.illum = illum;
		para.Ni = Ni;
		para.Ns = Ns;
		renderMng.RenderMaterialChange(basicMng, basicMng.materialsManager.currentMtlId, para,mtlType);
	}
	ImGui::SameLine();
	if (ImGui::Button("Exit")) {  show_materila_change_view = false; }
	ImGui::EndChild();

	ImGui::EndGroup();
	
	ImGui::End();
}