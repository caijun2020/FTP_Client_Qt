# FTP_Client_Qt

Development by Qt 4.8.1


Version: V1.0 2020-Aug-29
1. Bugfix for upload whole directory if the dir is subdirs. cdToParent will be called even though upload files.
  

Version: V1.0 2020-Aug-28
1. Implement the basic FTP client functions(login, download/upload files)
2. FTP client class and UI widget class are independent
3. Support to upload the whole directory(all files in dir, currently not support files in subdirectories)
4. Path and file name which contains Chinese characters are not supported currently

