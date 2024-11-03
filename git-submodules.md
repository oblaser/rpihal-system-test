# Git Submodules Cheatsheet



## Cloning a Repo with Submodules
https://git-scm.com/book/en/v2/Git-Tools-Submodules#_cloning_submodules
either
```sh
git clone --recurse-submodules https://github.com/oblaser/rpihal-system-test.git
```
or
```sh
git clone https://github.com/oblaser/rpihal-system-test.git
cd rpihal-system-test
git submodule init
git submodule update
```



## Add a Submodule (e.g. when creating a new repo)
https://git-scm.com/book/en/v2/Git-Tools-Submodules#_starting_submodules

Beeing in the repo root:
```sh
git submodule add https://github.com/oblaser/omw.git sdk/omw
git submodule add https://github.com/oblaser/rpihal.git sdk/rpihal
```
