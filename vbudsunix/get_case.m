function case_data = get_case(load_case);

file_name=horzcat('save',num2str(load_case),'.mat');
a=open(file_name);
case_data=a.data;

