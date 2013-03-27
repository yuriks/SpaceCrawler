@set builder=Debug\SpaceCrawler.exe -t

rmdir /s /q data
mkdir data

%builder% --pack-sprites data_src\ui\ data\ui
%builder% --pack-sprites data_src\background\ data\background
copy data_src\bullets.png data\
copy data_src\debug.png data\
copy data_src\ships.png data\

pause
