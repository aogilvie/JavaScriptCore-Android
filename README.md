# JavaScriptCore-Android

## About

**JavaScript Source Marker:** Cloned from http://android.googlesource.com/ master branch at Jan 29 2012.

## License

Please be ware that JavaScriptCore is open source under LGPL license!

## Compiling

#### iOS
Just open `jni/Source/JavaScriptCore/JavaScriptCore.xcodeproj` and compile.

#### Android

**With NDK:**

Run `/your_path_to/android-ndk-r9/ndk_build` at root of project.

**With Eclipse:**

*Requires CDT and ADT plugins*

- Open Eclipse
- File > New > Project... Android Project from Existing Code
- Right click project name > Properties > Builders
- Click New... > Program
- Fill out as follows:
	- Name: NDK Build
	- Location: select the path to ndk-build
	- Working Directory: *Browse Workspace* and select HelloJni project
	- Select Refrect Tab:
		- Tick: *Refresh resources upon completion*
		- Tick: *Recursively include sub-folders*
		- Click *Specify Resources* and select HelloJni > libs folder
	- Select Build Options Tab:
		- Tick *Specify working set of relevant resource* and click *Specify Resources...* select HelloJni > jni folder
- Build project

#### Win32

- Install 'Safari' browser for win32 ([download link](http://www.apple.com/safari/download/)). This step is necessary, otherwise, jsc application cannot be launched.

- Set environment variable

	- name: `WEBKITLIBRARIESDIR`
	- value: `F:/Project/JavaScriptCore-Android/jni/WebKitLibraries/win` (Please replace 'F:\Project' to your project's root path)

	- name: `WEBKITOUTPUTDIR`
	- value: `F:/Project/JavaScriptCore-Android/jni/WebKitBuild` (Please replace `F:\Project` to your project's root path)

	- name: SystemDrive
	- value: `E:`   (Please replace 'E:' to the root path of cygwin)

- Logout or reboot your computer, you can not start vs2008 if you don't do this.

- Open `JavaScriptCore-Android/jni/Source/JavaScriptCore/JavaScriptCore.vcproj/JavaScriptCore.sln` by VS2008 and build all projects.



## Authors

##### Cocos-2D Team

- [JianHua Chen](https://github.com/dumganhar‎) - Original publisher, Cocos-2D Team
- [Walzer Wang](https://github.com/walzer/) - Original publisher, Cocos-2D Team
- [markshiz](https://github.com/markshiz/) - author of Android port of [corefoundation](https://github.com/markshiz/corefoundation-lite-android)
- [aogilvie](https://github.com/aogilvie/) - Modifications and updates

### TODO

- Change the application's name from HelloJNI to HelloJSC
- Add other JSC test cases
- I'm not sure if we can leave the generated .lut.h files in runtime folder instead of generating them before compilation.
- I hacked jsc-android/jni/Source/JavaScriptCore/ctuils/Android.mk line 68 ~ line 82. If we build it on windows cygwin, only uio.c is involved. But on mac, WINDOWS_HOST_ONLY equal to 0 and mspace.c is involved, which leads to an error about bionic dependency. I hacked here to avoid mspace.c. Any how, libjs.so seems working well currently.
- gcc would raise an error if jsc-android/jni/Application.mk hasn't defined APP_OPTIM := release. I found the solution from here http://groups.google.com/group/android-ndk/browse_thread/thread/4dc0addd20cf23d0?tvc=2
