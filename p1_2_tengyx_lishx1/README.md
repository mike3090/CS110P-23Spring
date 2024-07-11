## Command line instructions
You can also upload existing files from your computer using the instructions below.

### Git global setup
```
git config --global user.name "lishx1"
git config --global user.email "lishx1@shanghaitech.edu.cn"
```

### Create a new repository
```
git clone https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p1.2_tengyx_lishx1.git
cd p1.2_tengyx_lishx1
git switch -c main
touch README.md
git add README.md
git commit -m "add README"
git push -u origin main
```

### Push an existing folder
```
cd existing_folder
git init --initial-branch=main
git remote add origin https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p1.2_tengyx_lishx1.git
git add .
git commit -m "Initial commit"
git push -u origin main
```

### Push an existing Git repository
```
cd existing_repo
git remote rename origin old-origin
git remote add origin https://autolab.sist.shanghaitech.edu.cn/gitlab/cs110_23s_projects/p1.2_tengyx_lishx1.git
git push -u origin --all
git push -u origin --tags
```