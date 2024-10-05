# 编译MS Terminal手记

```
mkdir wterm
cd wterm
git clone https://github.com/microsoft/terminal.git
cd terminal
git submodule update --init --recursive
```

Open OpenConsole.sln by using VSTS2022
```
To debug the Windows Terminal in VS, right click on CascadiaPackage (in the Solution Explorer) and go to properties. In the Debug menu, change "Application process" and "Background task process" to "Native Only".

You should then be able to build & debug the Terminal project by hitting F5. Make sure to select either the "x64" or the "x86" platform - the Terminal doesn't build for "Any Cpu" (because the Terminal is a C++ application, not a C# one).
```

