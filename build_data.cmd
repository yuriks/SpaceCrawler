@set builder=Debug\SpaceCrawler.exe -t

rmdir /s /q data
mkdir data

%builder% --pack-sprites data_src\ui\ data\ui_new
copy data_src\background.png data\
copy data_src\bullets.png data\
copy data_src\debug.png data\
copy data_src\ships.png data\
copy data_src\ui.png data\
