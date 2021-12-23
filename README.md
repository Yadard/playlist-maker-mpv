# playlist-maker-mpv
## Index:
 - [Installation](#Installation)
 - [Usage](#Usage)

----

<a name="Installation"></a>
## Installation
**This repo has submodules remember to git clone recursively**

A simple cmake build
clone recursively the repo
```bash
git clone --recursive https://github.com/Yadard/playlist-maker-mpv.git
cd playlist-maker-mpv
```
and build with cmake
``` bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```
and compile with the generator you chosed.


<a name="Usage"></a>
## Usage
This program assumes that you episodes will be stored in the following manner, ``${anyFolder}/${series_title}/${season_name}/${episodes}``, it's required that the videos are named with ``${series_title}.*${season_name}?.*${episode_number}`` that can have any separator **(' ', '-', '_', ...)**.

For example, the following structure will match:

``${anyFolder}/my Series/S1/${episodes} //running program inside S1``
```
//the matching is case insentive
.*my.*series.*${chapter_num}.*
.*my.*series.*S1.*${chapter_num}.*
```

**for summary**: as long as you have in your ``./../..`` folder the name of the series you are watching everything should work fine, if not open a issue. If you have any feature request open a issue with the tag feature-request.