# ubt-learn-project

git远程仓库技术博客地址，https://www.cnblogs.com/wangmingshun/p/5424767.html


先有本地仓库，再创建远程仓库
		第1步：创建SSH Key。在用户主目录下，看看有没有.ssh目录，如果有，再看看这个目录下有没有id_rsa和id_rsa.pub这两个文件，如果已经有了，可直接跳到下一步。如果没有，打开Shell（Windows下打开Git Bash），创建SSH Key：
		$ ssh-keygen -t rsa -C "461514274@qq.com"
		第2步：登陆GitHub，点击头像旁的下拉列表，选择“Account settings”，“SSH and GPG Keys”页面：
		然后，点“New SSH Key”，填上任意Title，在Key文本框里粘贴id_rsa.pub文件的内容：
		本地创建了一个Git仓库后，又想在GitHub创建一个Git仓库，并且让这两个仓库进行远程同步，这样，GitHub上的仓库既可以作为备份，又可以让其他人通过该仓库来协作
		登陆GitHub，然后，在右上角找到“New repository”按钮，创建一个新的仓库：

		自己初始化git init learn.git后，可以远程关联github项目上的learn.git
 		git remote add origin https://github.com/jian61n/learn.git

 		由于远程库是空的，我们第一次推送master分支时 git push -u origin master


上次我们讲了先有本地库，后有远程库的时候，如何关联远程库。
现在，假设我们从零开发，那么最好的方式是先创建远程库，然后，从远程库克隆。
	首先，登陆GitHub，创建一个新的仓库，名字叫downloadgit：
	我们勾选Initialize this repository with a README，这样GitHub会自动为我们创建一个README.md文件。创建完毕后，可以看到README.md文件：
	现在，远程库已经准备好了，下一步是用命令git clone克隆一个本地库：


	填上任意Title，在Key文本框里粘贴id_rsa.pub文件的内容：
	点“Add ssh Key”，你就应该看到已经添加的Key.
	为什么GitHub需要SSH Key呢？因为GitHub需要识别出你推送的提交确实是你推送的，而不是别人冒充的，而Git支持SSH协议，所以，GitHub只要知道了你的公钥，就可以确认只有你自己才能推送。
	当然，多人进行合作开发，只需要将每个人的key都重复上面的步骤将添加到你的github就可以了，这样他们也可以往你的github上推送内容。


技术博客  https://www.cnblogs.com/zxtceq/p/6963964.html