



R=dir

j=0;
for i=1:1:100


  if strfind(R(i).name,'save')
    j=j+1;
    R(i).name
    load(R(i).name)
    RR{j}=R(i).name;
    D(j)=data(21);
   end
   clear data
end