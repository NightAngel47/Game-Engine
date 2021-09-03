import os
import subprocess
import sys
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen
from zipfile import ZipFile

VULKAN_SDK = os.environ.get('VULKAN_SDK')
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/1.2.170.0/windows/vulkan_sdk.exe'
ENGINE_VULKAN_VERSION = '1.2.170.0'
VULKAN_SDK_EXE_PATH = 'GameEngine/vendor/VulkanSDK/VulkanSDK.exe'

def InstallVulkanSDK():
	print('Downloading {} to {}'.format(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH))
	Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
	print("Done!")
	print("Running Vulkan SDK Installer...")
	os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
	print("Re-run this script after installation")

def InstallVulkanPrompt():
	print("Would you like ot install the Vulkan SDK?")
	install = Utils.YesOrNo()
	if install:
		InstallVulkanSDK()
		quit()

def CheckVulkanSDK():
	if VULKAN_SDK is None:
		print("You don't have the Vulkan SDK installed!")
		InstallVulkanSDK()
		return False
	elif ENGINE_VULKAN_VERSION not in VULKAN_SDK:
		print(f"Located Vulkan SDK at {VULKAN_SDK}")
		print(f"You don't have the correct Vulkan SDK version! (Engine requires {ENGINE_VULKAN_VERSION})")
		return False

	print(f"Correct Vulkan SDK located at {VULKAN_SDK}")
	return True

VulkanSDKDebugLibURL = 'https://files.lunarg.com/SDK-1.2.170.0/VulkanSDK-1.2.170.0-DebugLibs.zip'
OutputDirectory = "GameEngine/vendor/VulkanSDK"
TempZipFile = f"{OutputDirectory}/VulkanSDK.zip"

def CheckVulkanSDKDebugLibs():
	shadercdLib = Path(f"{OutputDirectory}/Lib/shaderc_sharedd.lib")
	if (not shadercdLib.exists()):
		print(f"No Vulkan SDK debug libs found. (Checked {shadercdLib})")
		print("Downloading", VulkanSDKDebugLibURL)
		with urlopen(VulkanSDKDebugLibURL) as zipresp:
			with ZipFile(BytesIO(zipresp.read())) as zFile:
				zFile.extractall(OutputDirectory)

	print(f"Vulkan SDK debug libs located at {OutputDirectory}")
	return True
