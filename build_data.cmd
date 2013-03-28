@set builder=Debug\SpaceCrawler.exe -t

rmdir /s /q data
mkdir data

%builder% --pack-sprites data_src\ui\ data\ui
%builder% --pack-sprites data_src\background\ data\background
%builder% --pack-sprites data_src\bullets\ data\bullets
copy data_src\debug.png data\
%builder% --pack-sprites data_src\ships\ data\ships

pause
