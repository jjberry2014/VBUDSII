clear;
load_case=input('Load sample case # (<ENTER> to skip)? ');
compareto_flag=0;
% coarse_or_fine=1;   % 0 = coarse, 1 = fine
% coarse_or_fine = input('Coarse [0] or Fine [1] resolution for spectra? ');
coarse_or_fine=0;
% the below can be used to compare outputs from other code packages in the
% actinide buildup figures
if(compareto_flag==1)
    load('mcnpx_imf');
    mbu=mcnpx_imf(:,1);
    no_iso=size(mcnpx_imf);
    m_iso=zeros(no_iso(1),24);
    for(i=1:no_iso(2)-1)
        m_iso(:,i)=mcnpx_imf(:,i+1);
    end
end
clf reset;
close all;
nfigsx=3;
nfigsy=3;
out1=VBUDSgui(load_case);
movegui(out1,'center');
h1=guihandles(out1);
waitfor(h1.pushbutton1,'String','Wait...');
rx_name=get(h1.edit1,'String');
geom=get(h1.popupmenu1,'Value');
fuel_region=get(h1.radiobutton6,'Value');
comp=zeros(2,5);
rho=zeros(2,5);
vf=zeros(2,5);
vf_pert=zeros(2,5);
comp(fuel_region+1,1)=get(h1.popupmenu2,'Value');
rho(fuel_region+1,1)=str2double(get(h1.edit3,'String'));
vf(fuel_region+1,1)=str2double(get(h1.edit4,'String'));
comp(1,1+(1-fuel_region))=get(h1.popupmenu3,'Value')-1;
comp(1,2+(1-fuel_region))=get(h1.popupmenu4,'Value')-1;
comp(1,3+(1-fuel_region))=get(h1.popupmenu5,'Value')-1;
comp(1,4+(1-fuel_region))=get(h1.popupmenu6,'Value')-1;
comp(2,1+(fuel_region))=get(h1.popupmenu7,'Value')-1;
comp(2,2+(fuel_region))=get(h1.popupmenu8,'Value')-1;
comp(2,3+(fuel_region))=get(h1.popupmenu9,'Value')-1;
comp(2,4+(fuel_region))=get(h1.popupmenu10,'Value')-1;
for(i=1:5)
    for(j=1:2)
        if(j~=fuel_region+1 || i~= 1)
            if(comp(j,i)==1 || comp(j,i)==2 || ...
               comp(j,i)==4 || comp(j,i)==5 || ... 
               comp(j,i)==6 || comp(j,i)==7 || ...
               comp(j,i)==8)
                 vf_pert(j,i)=1;
            end
        end
    end
end
ca_flag=get(h1.popupmenu11,'Value')-1;
vf(1,1+(1-fuel_region))=str2double(get(h1.edit8,'String'));
vf(1,2+(1-fuel_region))=str2double(get(h1.edit10,'String'));
vf(1,3+(1-fuel_region))=str2double(get(h1.edit12,'String'));
vf(1,4+(1-fuel_region))=str2double(get(h1.edit14,'String'));
vf(2,1+(fuel_region))=str2double(get(h1.edit16,'String'));
vf(2,2+(fuel_region))=str2double(get(h1.edit18,'String'));
vf(2,3+(fuel_region))=str2double(get(h1.edit20,'String'));
vf(2,4+(fuel_region))=str2double(get(h1.edit22,'String'));
rho(1,1+(1-fuel_region))=str2double(get(h1.edit9,'String'));
rho(1,2+(1-fuel_region))=str2double(get(h1.edit11,'String'));
rho(1,3+(1-fuel_region))=str2double(get(h1.edit13,'String'));
rho(1,4+(1-fuel_region))=str2double(get(h1.edit15,'String'));
rho(2,1+(fuel_region))=str2double(get(h1.edit17,'String'));
rho(2,2+(fuel_region))=str2double(get(h1.edit19,'String'));
rho(2,3+(fuel_region))=str2double(get(h1.edit21,'String'));
rho(2,4+(fuel_region))=str2double(get(h1.edit23,'String'));
shim(1)=get(h1.checkbox2,'Value');
shim(2)=get(h1.checkbox3,'Value');
shimconc=-str2double(get(h1.edit52,'String'));
a_spec=get(h1.radiobutton1,'Value');
if(a_spec < 1)
    a_spec = -1;
else
    a_spec = 1;
end
actinides=zeros(24,1);
actinides(13)=a_spec*str2double(get(h1.edit24,'String'));
actinides(14)=a_spec*str2double(get(h1.edit25,'String'));
actinides(15)=a_spec*str2double(get(h1.edit26,'String'));
actinides(1)=a_spec*str2double(get(h1.edit27,'String'));
actinides(2)=a_spec*str2double(get(h1.edit28,'String'));
actinides(3)=a_spec*str2double(get(h1.edit29,'String'));
actinides(4)=a_spec*str2double(get(h1.edit30,'String'));
actinides(5)=a_spec*str2double(get(h1.edit31,'String'));
actinides(6)=a_spec*str2double(get(h1.edit32,'String'));
actinides(16)=a_spec*str2double(get(h1.edit33,'String'));
actinides(7)=a_spec*str2double(get(h1.edit34,'String'));
actinides(8)=a_spec*str2double(get(h1.edit35,'String'));
actinides(9)=a_spec*str2double(get(h1.edit36,'String'));
actinides(10)=a_spec*str2double(get(h1.edit37,'String'));
actinides(11)=a_spec*str2double(get(h1.edit38,'String'));
actinides(12)=a_spec*str2double(get(h1.edit39,'String'));
actinides(17)=a_spec*str2double(get(h1.edit40,'String'));
actinides(18)=a_spec*str2double(get(h1.edit41,'String'));
actinides(19)=a_spec*str2double(get(h1.edit42,'String'));
actinides(20)=a_spec*str2double(get(h1.edit43,'String'));
actinides(21)=a_spec*str2double(get(h1.edit44,'String'));
actinides(22)=a_spec*str2double(get(h1.edit45,'String'));
actinides(23)=a_spec*str2double(get(h1.edit46,'String'));
actinides(24)=a_spec*str2double(get(h1.edit47,'String'));

clad_comp=get(h1.popupmenu12,'Value');
struct_comp=get(h1.popupmenu13,'Value');
clad_rho=str2double(get(h1.edit53,'String'));
struct_rho=str2double(get(h1.edit54,'String'));
struct_vf=str2double(get(h1.edit55,'String'));
temps(1)=str2double(get(h1.edit56,'String'));
temps(2)=str2double(get(h1.edit58,'String'));
dimen(1)=str2double(get(h1.edit59,'String'));
dimen(2)=str2double(get(h1.edit60,'String'));
cell_geom(1)=str2double(get(h1.edit48,'String'));
cell_geom(2)=str2double(get(h1.edit49,'String'));
cell_geom(3)=str2double(get(h1.edit50,'String'));


out2=VBUDSplotgui(load_case);
movegui(out2,'center');
h2=guihandles(out2);
waitfor(h2.pushbutton1,'String','Wait...');
mode=get(h2.radiobutton2,'Value');
rxrate_cutoff=str2double(get(h2.edit5,'String'))/100.;
flux_step=str2double(get(h2.edit7,'String'));
burn_step=str2double(get(h2.edit8,'String'));
if(mode)
    plot_options(1)=get(h2.checkbox33,'Value');
    plot_options(2)=get(h2.checkbox34,'Value');
    plot_options(3)=get(h2.checkbox35,'Value');
    plot_options(4)=get(h2.checkbox36,'Value');
    plot_options(5)=get(h2.checkbox37,'Value');
    x_axis_option=get(h2.radiobutton4,'Value');
    disch_bu=str2double(get(h2.edit1,'String'));
    t_res=str2double(get(h2.edit2,'String'));
    n_batches=str2double(get(h2.edit3,'String'));
    load_factor=str2double(get(h2.edit4,'String'));
else
    plot_options(1)=get(h2.checkbox3,'Value');
    plot_options(2)=get(h2.checkbox4,'Value');
    plot_options(3)=get(h2.checkbox5,'Value');
    plot_options(4)=get(h2.checkbox6,'Value');
    plot_options(5)=get(h2.checkbox7,'Value');
    x_axis_option=0;
    disch_bu=0;
    t_res=0;
    n_batches=1.;
    load_factor=100.;
end
if(str2double(get(h2.edit9,'String')) > 0)
    a = load('catnames.mat');
    textdata = a.textdata;
    data = zeros(size(textdata));
    data(1)=2-get(h1.radiobutton5,'Value');
    data(2)=get(h1.popupmenu1,'Value');
    data(3)=get(h1.popupmenu2,'Value');
    if(get(h1.checkbox2,'Value') > 0)
        data(4)=1;
        data(5)=-shimconc;
    else 
        if(get(h1.checkbox3,'Value') > 0)
            data(4)=2;
            data(5)=-shimconc;
        else
            data(4)=0;
            data(5)=0;
        end
    end
    data(6)=get(h1.popupmenu11,'Value');
    data(7)=rho(fuel_region+1,1);
    data(8)=vf(fuel_region+1,1);
    data(9)=get(h1.popupmenu3,'Value');
    data(12)=get(h1.popupmenu4,'Value');
    data(15)=get(h1.popupmenu5,'Value');
    data(18)=get(h1.popupmenu6,'Value');
    data(21)=get(h1.popupmenu7,'Value');
    data(24)=get(h1.popupmenu8,'Value');
    data(27)=get(h1.popupmenu9,'Value');
    data(30)=get(h1.popupmenu10,'Value');
    data(11)=str2double(get(h1.edit8,'String'));
    data(10)=str2double(get(h1.edit9,'String'));    
    data(14)=str2double(get(h1.edit10,'String'));
    data(13)=str2double(get(h1.edit11,'String'));
    data(17)=str2double(get(h1.edit12,'String'));
    data(16)=str2double(get(h1.edit13,'String'));
    data(20)=str2double(get(h1.edit14,'String'));
    data(19)=str2double(get(h1.edit15,'String'));
    data(23)=str2double(get(h1.edit16,'String'));
    data(22)=str2double(get(h1.edit17,'String'));
    data(26)=str2double(get(h1.edit18,'String'));
    data(25)=str2double(get(h1.edit19,'String'));
    data(29)=str2double(get(h1.edit20,'String'));
    data(28)=str2double(get(h1.edit21,'String'));
    data(32)=str2double(get(h1.edit22,'String'));
    data(31)=str2double(get(h1.edit23,'String'));
    data(33)=get(h1.radiobutton1,'Value');
    if(data(33) < 1)
        data(33)=2;
    end
    data(34)=str2double(get(h1.edit24,'String'));
    data(35)=str2double(get(h1.edit25,'String'));
    data(36)=str2double(get(h1.edit26,'String'));
    data(37)=str2double(get(h1.edit27,'String'));
    data(38)=str2double(get(h1.edit28,'String'));
    data(39)=str2double(get(h1.edit29,'String'));
    data(40)=str2double(get(h1.edit30,'String'));
    data(41)=str2double(get(h1.edit31,'String'));
    data(42)=str2double(get(h1.edit32,'String'));
    data(43)=str2double(get(h1.edit33,'String'));
    data(44)=str2double(get(h1.edit34,'String'));
    data(45)=str2double(get(h1.edit35,'String'));
    data(46)=str2double(get(h1.edit36,'String'));
    data(47)=str2double(get(h1.edit37,'String'));
    data(48)=str2double(get(h1.edit38,'String'));
    data(49)=str2double(get(h1.edit39,'String'));
    data(50)=str2double(get(h1.edit40,'String'));
    data(51)=str2double(get(h1.edit41,'String'));
    data(52)=str2double(get(h1.edit42,'String'));
    data(53)=str2double(get(h1.edit43,'String'));
    data(54)=str2double(get(h1.edit44,'String'));
    data(55)=str2double(get(h1.edit45,'String'));
    data(56)=str2double(get(h1.edit46,'String'));
    data(57)=str2double(get(h1.edit47,'String'));
    data(58)=clad_comp;
    data(60)=struct_comp;
    data(59)=clad_rho;
    data(61)=struct_rho;
    data(62)=struct_vf;
    data(63)=temps(1);
    data(64)=temps(2);
    data(65)=dimen(1);
    data(66)=dimen(2);
    data(67)=str2double(get(h1.edit48,'String'));
    data(69)=str2double(get(h1.edit49,'String'));
    data(68)=str2double(get(h1.edit50,'String'));
    data(70)=mode+1;
    data(71)=rxrate_cutoff*100;
    if(mode)
        data(72)=disch_bu;
        data(73)=t_res;
        data(74)=n_batches;
        data(75)=load_factor; 
    else
        data(72)=0;
        data(73)=0;
        data(74)=0;
        data(75)=0;
    end
    save_fn=horzcat('save',get(h2.edit9,'String'),'.mat')
    save(save_fn,'textdata','data');
end

y_axis_option=get(h2.radiobutton6,'Value');
plot_act(1)=get(h2.checkbox8,'Value');
plot_act(2)=get(h2.checkbox9,'Value');
plot_act(3)=get(h2.checkbox10,'Value');
plot_act(4)=get(h2.checkbox11,'Value');
plot_act(5)=get(h2.checkbox12,'Value');
plot_act(6)=get(h2.checkbox13,'Value');
plot_act(7)=get(h2.checkbox14,'Value');
plot_act(8)=get(h2.checkbox15,'Value');
plot_act(9)=get(h2.checkbox16,'Value');
plot_act(10)=get(h2.checkbox17,'Value');
plot_act(11)=get(h2.checkbox18,'Value');
plot_act(12)=get(h2.checkbox20,'Value');
plot_act(13)=get(h2.checkbox21,'Value');
plot_act(14)=get(h2.checkbox22,'Value');
plot_act(15)=get(h2.checkbox23,'Value');
plot_act(16)=get(h2.checkbox24,'Value');
plot_act(17)=get(h2.checkbox25,'Value');
plot_act(18)=get(h2.checkbox26,'Value');
plot_act(19)=get(h2.checkbox19,'Value');
plot_act(20)=get(h2.checkbox27,'Value');
plot_act(21)=get(h2.checkbox28,'Value');
plot_act(22)=get(h2.checkbox29,'Value');
plot_act(23)=get(h2.checkbox30,'Value');
plot_act(24)=get(h2.checkbox31,'Value');

close(out1);
close(out2);

set(0,'Units','Pixels');
screen=get(0,'ScreenSize');
xpos=0;
ypos=0;
del_x=screen(3)/nfigsx;
del_y=screen(4)/nfigsy-25;
figloc=[xpos ypos del_x del_y];
act_names={'Th-230','Th-232','Pa-231','U-232','U-233','U-234','U-235','U-236',...
        'U-238','Np-237','Pu-236','Pu-238','Pu-239','Pu-240','Pu-241','Pu-242',...
        'Am-241','Am-242m','Am-243','Cm-242','Cm-243','Cm-244','Cm-245','Cm-246'};
act_weights=[230 232 231 232 233 234 235 236 238 237 236 238 239 240 241 242 ...
        241 242 243 242 243 244 245 246];
new_order=[13 14 15 1 2 3 4 5 6 16 7 8 9 10 11 12 17 18 19 20 21 22 23 24];
flip_order=[4 5 6 7 8 9 11 12 13 14 15 16 1 2 3 10 17 18 19 20 21 22 23 24]; 
fuel_types={'oxidefuel','carbidefuel','oxycarbidefuel','nitridefuel','fuel'};
nonfuel_types={'h2o','d2o','graphite','he-4','co2','lead','bismuth','sodium',...
        'zirconium','zro2','zrh','y2o3','mgo','nb'};
ca_types={'b4c','gd2o3','ag-in-cd'};
clad_types={'zircaloy'};
struct_types={'steel'};


if(x_axis_option)
    x_label='Burnup [MWd/kg]';
else
    x_label='Time [days]';
end
if(y_axis_option)
    y_label='Actinide Mass Fraction [% of IHM]';
else
    y_label='Number Density [atoms/barn-cm]';
end
if(mode)
    
    xaxis_labels={x_label,x_label,x_label,'Neutron Kinetic Energy [eV]','Neutron Kinetic Energy [eV]'};
    yaxis_labels={'Neutron Consumption Rate [n/s] Normalized to Unit Consumption',...
            'Neutron Production Rate [n/s] Normalized to Unit Consumption',...
            'K_{eff} and 6-Factor Coefficients [Dimensionless]',...
            'Neutron Flux [n/cm^{2}-s-leth], Arb. Scale',...
            'Neutron Flux [n/cm^{2}-s-leth], Arb. Scale'};        
    plot_options(3)=plot_options(2);
    plot_options(2)=plot_options(1);
else
    xaxis_labels={'','','','Neutron Kinetic Energy [eV]','Neutron Kinetic Energy [eV]'};
    yaxis_labels={'Neutron Production Rate [n/s] Normalized to Unit Consumption',...
            'K_{eff} and 6-Factor Coefficients [Dimensionless]',...
            'Reactivity Coefficient [10^{-6}\Delta k/k]',...
            'Neutron Flux [n/cm^{2}-s-leth], Arb. Scale',...
            'Neutron Flux [n/cm^{2}-s-leth], Arb. Scale'};    
end
for(i=1:5)
    if(plot_options(i))
        h1=figure(i);
        set(h1,'Position',figloc);
        xlabel(xaxis_labels(i));
        ylabel(yaxis_labels(i));
        if(xpos + del_x < screen(3))
            figloc(1)=figloc(1)+del_x;
            xpos = xpos + del_x;
        else
            if (ypos + 2*del_y < screen(4))
                ypos = ypos + del_y;
                xpos=0;
                figloc(1)=xpos;
                figloc(2)=ypos;
            else
                xpos=0;
                ypos=0;
                figloc(1)=0;
                figloc(2)=0;
            end
        end
    end
end

if(mode)
    plot_options(2)=plot_options(3);
    plot_options(3)=0;
    for(i=1:length(act_names))
        if(plot_act(i))
            h1=figure(i+5);
            set(h1,'Position',figloc);
            xlabel(x_label);
            ylabel(y_label);
            title(act_names(i));
          if(xpos + del_x < screen(3))
                figloc(1)=figloc(1)+del_x;
                xpos = xpos + del_x;
            else
                if (ypos + 2*del_y < screen(4))
                    ypos = ypos + del_y;
                    xpos=0;
                    figloc(1)=xpos;
                    figloc(2)=ypos;
                else
                   xpos=0;
                    ypos=0;
                    figloc(1)=0;
                    figloc(2)=0;
                end
            end
        end
    end
end
% save inputDeck1002;
%Altered by Bob Jan 2 2011
% mex VBUDS_solve_rx.c

   

if(mode==0) 

%HERE IS WHERE BOB TWEAKED
    compp=comp'; rhop=rho';vfp=vf'; actinidesp=actinides';
    save('variables_before_vbuds.mat');
    %make certain system('make') has been run
    disp('cld checkpoint VBUDS 1');
    system('./vbuds');
    disp('cld checkpoint VBUDS 2');
    load ('variables_after_vbuds.mat');
    nucl_names=vbuds1; nucl_int_rates=vbuds2; six_factors=vbuds3; VBUDS_flux=vbuds4; VBUDS_fineflux=vbuds5;
    
%[nucl_names nucl_int_rates six_factors VBUDS_flux VBUDS_fineflux] = ...
%VBUDS_solve_rx(rx_name,geom,fuel_region,comp',rho',vf',shim,shimconc,...
%    actinides',clad_comp,clad_rho,struct_comp,struct_rho,struct_vf,temps,...
%    dimen,cell_geom,load_factor,fuel_types,nonfuel_types,ca_types,...
%    ca_flag,clad_types,struct_types,mode,x_axis_option,y_axis_option,...
%    plot_options,disch_bu,t_res,n_batches,load_factor,flux_step,burn_step);
%END BOB INFLUENCE



abs_rates = nucl_int_rates(:,1);
fn_rates = nucl_int_rates(:,3);
sort_names = nucl_names;
bar_rates = [abs_rates fn_rates];
tot_abs = sum(abs_rates);
tot_fn = sum(fn_rates);
done=false;
fail=false;
while (~done)
  for i=1:length(sort_names)-1
    for(j=i+1:length(sort_names))
      if(sort_names(i,:)==sort_names(j,:))
        sort_names(i,:)=[];
        bar_rates(j,:)=bar_rates(j,:)+bar_rates(i,:);
        bar_rates(i,:)=[];
        fail=true;
        i=1;
        break;
      end
      if(fail)
        break;
      end
      if(i==length(sort_names)-1) 
        done=true;
      end
    end
    if(fail)
        fail=false;
        break;
    end
  end
end
out_of_bounds=intersect(find((bar_rates(:,1)/tot_abs)<rxrate_cutoff),...
    find((bar_rates(:,2)/tot_fn)<rxrate_cutoff));
bar_rates(out_of_bounds,:)=[];
sort_names(out_of_bounds,:)=[];
ded_abs=sum(bar_rates(:,1));
ded_fn=sum(bar_rates(:,2));
index1=length(bar_rates(:,1))+1;
sort_names(index1,1)='O';
sort_names(index1,2)='t';
sort_names(index1,3)='h';
sort_names(index1,4)='e';
sort_names(index1,5)='r';
bar_rates(index1,:)=[tot_abs-ded_abs tot_fn-ded_fn];
bar_rates(:,1)=bar_rates(:,1)/tot_abs;
bar_rates(:,2)=bar_rates(:,2)/tot_abs;
if(mode==0 && plot_options(1))
    figure(1);
    hold on;
    colormap('gray');
    bar(bar_rates','stacked');
    xlim([.4 2.6]);
    legend(sort_names);
    set(figure(1),'Name','Production and Consumption by Species');
    set(gca,'xtick',[1 2]);
    set(gca,'xticklabel',{'Consumption';'Production'});
end
if(mode==0 && plot_options(2))
    figure(2);
    hold on;
    white_bars=six_factors;
    blue_bars=six_factors;
    white_bars(7)=[];
    for(i=1:6)
        blue_bars(i)=0.;
    end
    barh(six_factors,'w');
    barh(blue_bars,'b');
    ylim([.2 7.8]);
    set(figure(2),'Name','Keff and 6-Factor Formula Terms');
    set(gca,'ytick',[]);
    labels={'\eta , Reproduction Factor',...
            '{\itf}, Thermal Utilization',...
            '{\itp}, Resonance Escape Probability',...
            '\epsilon , Fast Fission Factor',...
            '{\itL}_{f}, Fast Nonleakage Probability',...
            '{\itL}_{t}, Thermal Nonleakage Probability',...
            'K_{eff}, Multiplication Factor'};
    for(i=1:7)
        text(0.02,i,labels(i));
        text(six_factors(i)+0.02,i,{six_factors(i)});
    end
end
disp('cld checkpoint VBUDS 3');
if(mode==0 && (plot_options(4) || plot_options(5)))
    if(coarse_or_fine)
        flux=[VBUDS_fineflux(:,1) VBUDS_fineflux(:,2)];
        energy=VBUDS_fineflux(:,3);
    else
        flux=[VBUDS_flux(:,1) VBUDS_flux(:,2)];
        energy=VBUDS_flux(:,3);
    end
    if(plot_options(4))
        figure(4);
        hold on;
        plot(energy,flux(:,1),'color','r');
        set(figure(4),'Name','Flux Spectrum in Central Region');
        set(gca,'XScale','log');
        set(gca,'YScale','log');
        xlim([1e-3 1e7]);
        ylim([min(flux(:,1))*1000 max(flux(:,1))*2]);
        set(gca,'xtick',[1e-3 1e-2 1e-1 1 1e1 1e2 1e3 1e4 1e5 1e6 1e7]);
    end
    if(plot_options(5))
        figure(5);
        hold on;
        plot(energy,flux(:,2),'color','b');
        set(figure(5),'Name','Flux Spectrum in Annular Region');
        set(gca,'XScale','log');
        set(gca,'YScale','log');
        xlim([1e-3 1e7]);
        ylim([min(flux(:,2))*1000 max(flux(:,2))*2]);
        set(gca,'xtick',[1e-3 1e-2 1e-1 1 1e1 1e2 1e3 1e4 1e5 1e6 1e7]);
    end
end
disp('cld checkpoint VBUDS 4');
if(mode==0 && plot_options(3))
    k_orig=six_factors(7);
    if(temps(1)> 949)
        temps(1) = temps(1)-50.;
        delta_t = -50.;
    else
        temps(1) = temps(1)+50.;
        delta_t = 50.;
    end
 %HERE IS WHERE BOB TWEAKED
    compp=comp'; rhop=rho';vfp=vf'; actinidesp=actinides';
    save('variables_before_vbuds.mat');
    %make certain system('make') has been run
    system('./vbuds');
    load ('variables_after_vbuds.mat');
    nucl_names0=vbuds1; nucl_int_rates0=vbuds2; six_factors0=vbuds3; VBUDS_flux0=vbuds4; VBUDS_fineflux0=vbuds5;
    
%    [nucl_names0 nucl_int_rates0 six_factors0 VBUDS_flux0 VBUDS_fineflux0] = ...
%    VBUDS_solve_rx(rx_name,geom,fuel_region,comp',rho',vf',shim,shimconc,...
%    actinides',clad_comp,clad_rho,struct_comp,struct_rho,struct_vf,temps,...
%    dimen,cell_geom,load_factor,fuel_types,nonfuel_types,ca_types,...
%    ca_flag,clad_types,struct_types,mode,x_axis_option,y_axis_option,...
%    plot_options,disch_bu,t_res,n_batches,load_factor,flux_step,burn_step);
%END BOB INFLUENCE
    k_pert=six_factors0(7);
    alpha_t0=(1./k_orig)*(k_pert-k_orig)/delta_t;
    reac_c(1)=alpha_t0;
    temps(1) = temps(1) - delta_t;
    if(temps(2)> 949)
        temps(2) = temps(2)-50.;
        delta_t = -50.;
    else
        temps(2) = temps(2)+50.;
        delta_t = 50.;
    end
    
 %HERE IS WHERE BOB TWEAKED
    compp=comp'; rhop=rho';vfp=vf'; actinidesp=actinides';
    save('variables_before_vbuds.mat');
    %make certain system('make') has been run
    system('./vbuds');
    load ('variables_after_vbuds.mat');
    nucl_names1=vbuds1; nucl_int_rates1=vbuds2; six_factors1=vbuds3; VBUDS_flux1=vbuds4; VBUDS_fineflux0=vbuds5;
           
%    [nucl_names1 nucl_int_rates1 six_factors1 VBUDS_flux1 VBUDS_fineflux0] = ...
%    VBUDS_solve_rx(rx_name,geom,fuel_region,comp',rho',vf',shim,shimconc,...
%    actinides',clad_comp,clad_rho,struct_comp,struct_rho,struct_vf,temps,...
%    dimen,cell_geom,load_factor,fuel_types,nonfuel_types,ca_types,...
%    ca_flag,clad_types,struct_types,mode,x_axis_option,y_axis_option,...
%    plot_options,disch_bu,t_res,n_batches,load_factor,flux_step,burn_step);
%END BOB INFLUENCE
    k_pert=six_factors1(7);    
    alpha_t1=(1./k_orig)*(k_pert-k_orig)/delta_t;
    reac_c(2)=alpha_t1;
    temps(2) = temps(2) - delta_t;
    delta_vf=0.;
    new_vf=vf;
    for(i=1:5)
        for(j=1:2)
            if(vf_pert(j,i))
                new_vf(j,i)=vf(j,i)*.9;
                delta_vf=(vf(j,i)-new_vf(j,i))/(vf(j,i)/100.);
            end
        end
    end
    
%HERE IS WHERE BOB TWEAKED
    compp=comp'; rhop=rho';vfp=vf'; actinidesp=actinides';
    save('variables_before_vbuds.mat');
    %make certain system('make') has been run
    system('./vbuds');
    load('variables_after_vbuds.mat');
    nucl_names2=vbuds1; nucl_int_rates2=vbuds2; six_factors2=vbuds3; VBUDS_flux2=vbuds4; VBUDS_fineflux0=vbuds5;
    
%    [nucl_names2 nucl_int_rates2 six_factors2 VBUDS_flux2 VBUDS_fineflux0] = ...
%    VBUDS_solve_rx(rx_name,geom,fuel_region,comp',rho',new_vf',shim,shimconc,...
%    actinides',clad_comp,clad_rho,struct_comp,struct_rho,struct_vf,temps,...
%    dimen,cell_geom,load_factor,fuel_types,nonfuel_types,ca_types,...
%    ca_flag,clad_types,struct_types,mode,x_axis_option,y_axis_option,...
%    plot_options,disch_bu,t_res,n_batches,load_factor,flux_step,burn_step);
%END BOB INFLUENCE
    k_pert=six_factors2(7);   
    rho_orig=(k_orig-1.)/k_orig;
    rho_pert=(k_pert-1.)/k_pert;
    alpha_vf=(rho_pert-rho_orig)/delta_vf/100.;
    reac_c(3)=alpha_vf;   
    figure(3);
    hold on;
    bar(reac_c*1e6);
    set(figure(3),'Name','Reactivity Coefficients');
    ylim([-max(abs(reac_c*1e6))*1.2 max(abs(reac_c*1e6))*1.2]);
    xlim([0.2 0.8+length(reac_c)]);
    plot([0.2 0.8+length(reac_c)],[0 0],'color','k');
    set(gca,'xtick',[1 2 3]);
    set(gca,'xticklabel',{'Central Reg Temp[1/K]';'Annular Reg Temp[1/K]';...
            'Void [1/(0.01 % void)]'});
end
else 
    if (mode==1) 
%HERE IS WHERE BOB TWEAKED
    compp=comp'; rhop=rho';vfp=vf'; actinidesp=actinides';
    save('variables_before_vbuds.mat');
    %make certain system('make') has been run
    system('./vbuds');
    load ('variables_after_vbuds.mat');
    nucl_names=vbuds1; nucl_int_rates=vbuds2; six_factors=vbuds3; VBUDS_flux=vbuds4; VBUDS_e=vbuds5; coarse_t=vbuds6; fine_t=vbuds7; n_act=vbuds8; VBUDS_fineflux=vbuds9; VBUDS_finee=vbuds10;
    
%    [nucl_names nucl_int_rates six_factors VBUDS_flux VBUDS_e coarse_t fine_t n_act VBUDS_fineflux VBUDS_finee] = ...
%    VBUDS_solve_rx(rx_name,geom,fuel_region,comp',rho',vf',shim,shimconc,...
%    actinides',clad_comp,clad_rho,struct_comp,struct_rho,struct_vf,temps,...
%    dimen,cell_geom,load_factor,fuel_types,nonfuel_types,ca_types,...
%    ca_flag,clad_types,struct_types,mode,x_axis_option,y_axis_option,...
%    plot_options,disch_bu,t_res,n_batches,load_factor,flux_step,burn_step);
%End Bob Influence      
    names1=nucl_names;
    intrates1=nucl_int_rates;
    sf1=six_factors;
    if(coarse_or_fine)
        f1=VBUDS_fineflux;
        e1=VBUDS_finee;
    else
        f1=VBUDS_flux;
        e1=VBUDS_e;
    end
    
    out_of_bounds=find(fine_t(:,1)<0);
    fine_t(out_of_bounds,:)=[];
    n_act(:,out_of_bounds)=[];

    ct=coarse_t;
    a1=n_act;
    ft=fine_t;
    a2=a1;
    
    for(h=1:length(act_names))
        a2(h,:)=a2(h,:)*act_weights(flip_order(h));
    end
    tot_act_nd=sum(a2(:,1));
    a2=a2./tot_act_nd.*100.;

    out_of_bounds=find(sf1(7,:)<0.0001);
    intrates1(:,:,out_of_bounds)=[];
    sf1(:,out_of_bounds)=[];
    f1(:,:,out_of_bounds)=[];
    ct(out_of_bounds,:)=[];
    
    dd=size(intrates1);
    bar_rates=zeros([dd(1) 2 dd(3)]);
    bar_rates(:,1,:)=intrates1(:,1,:);
    bar_rates(:,2,:)=intrates1(:,3,:);
    sort_names=names1;
    for i=1:length(bar_rates(:,1,1))
        max_abs(i)=max(bar_rates(i,1,:));
        max_fn(i)=max(bar_rates(i,2,:));
    end
    for i=1:length(bar_rates(1,1,:))
        tot_abs(i)=sum(bar_rates(:,1,i));
        tot_fn(i)=sum(bar_rates(:,2,i));
    end
    norm_abs=max(tot_abs);
    norm_fn=max(tot_fn);
    out_of_bounds=intersect(find((max_abs/norm_abs)<rxrate_cutoff),...
    find((max_fn/norm_fn)<rxrate_cutoff));
    bar_rates(out_of_bounds,:,:)=[];
    names1(out_of_bounds,:)=[];
    ded_abs=sum(bar_rates(:,1,:));
    ded_abs=ded_abs(:);
    ded_fn=sum(bar_rates(:,2,:));
    ded_fn=ded_fn(:);
    stupido=size(names1);
    index1=stupido(1)+1;
    names1(index1,1)='O';
    names1(index1,2)='t';
    names1(index1,3)='h';
    names1(index1,4)='e';
    names1(index1,5)='r';
    bar_rates(index1,:,:)=zeros;
    bar_rates(index1,1,:)=[tot_abs-ded_abs'];
    bar_rates(index1,2,:)=[tot_fn-ded_fn'];
    for i=1:dd(3)
      bar_rates(:,1,i)=bar_rates(:,1,i)/tot_abs(i);
      bar_rates(:,2,i)=bar_rates(:,2,i)./tot_abs(i);
    end
    if(x_axis_option)
        domain=ct(:,2);
        fine_d=ft(:,2);
    else
        domain=ct(:,1);
        fine_d=ft(:,1);
    end
    if(plot_options(1))
        figure(1);
        set(figure(1),'Name','Neutron Consumption by Species');
        hold on;
        midp=round(length(domain)/2);
        for i=1:index1
          curve1=bar_rates(i,1,:);
          curve1=curve1(:);
          plot(domain,curve1);
          text(domain(midp),curve1(midp),names1(i,:));
        end
        plot(domain,tot_abs./tot_abs);
        set(gca,'yscale','log');
        ylim([rxrate_cutoff/1.10 1.10]);
        text(domain(midp),1.02,'Total');
        
        figure(2);
        set(figure(2),'Name','Neutron Production by Species');
        hold on;
        for i=1:index1
          curve1=bar_rates(i,2,:);
          curve1=curve1(:);
          plot(domain,curve1);
          text(domain(midp),curve1(midp),names1(i,:));
        end
        plot(domain,tot_fn./tot_abs);
        set(gca,'yscale','log');
        ylim([rxrate_cutoff/1.10 max(tot_fn./tot_abs)*1.05]);
        text(domain(midp),tot_fn(midp)/tot_abs(midp),'Total');
    end
    if(plot_options(2))
        figure(3);
        set(figure(3),'Name','Keff and Six-Factor Formula Terms');
        hold on;
        plot(domain,sf1,'LineWidth',1.75);
        maxrange=max(max(sf1));
        set(gca,'ylim',[0 maxrange*1.1]);
        labels={'\eta , Reproduction Factor',...
            '{\itf}, Thermal Utilization',...
            '{\itp}, Resonance Escape Probability',...
            '\epsilon , Fast Fission Factor',...
            '{\itL}_{f}, Fast Nonleakage Probability',...
            '{\itL}_{t}, Thermal Nonleakage Probability',...
            'K_{eff}, Multiplication Factor'};
        legend(labels,3);
    end
    if(plot_options(4))
        figure(4);
        flux0=f1(:,1,:);
        flux0=flux0(:,:);
        ymin=min(min(flux0))*1000;
        ymax=max(max(flux0))*2;
        n_frames=size(flux0);
        n_frames=n_frames(2);
        for i=1:n_frames
            plot(e1,flux0(:,i));
            set(gca,'XScale','log');
            set(gca,'YScale','log');
            xlim([1e-3 1e7]);
            ylim([ymin ymax]);
            text(1e1,ymin*6,'Time [d]');
            text(3e3,ymin*6,'Burnup [MWd/kg]');
            text(1e1,ymin*4,{ct(i,1)});
            text(3e3,ymin*4,{ct(i,2)});
            set(gca,'xtick',[1e-3 1e-2 1e-1 1 1e1 1e2 1e3 1e4 1e5 1e6 1e7]);
            frame(i)=getframe;
        end
        set(figure(4),'Name','Flux Spectrum in Central Region');
    end
    if(plot_options(5))
        figure(5);
        flux1=f1(:,2,:);
        flux1=flux1(:,:);
        ymin=min(min(flux1))*1000;
        ymax=max(max(flux1))*2;
        n_frames=size(flux1);
        n_frames=n_frames(2);
        for i=1:n_frames
            plot(e1,flux1(:,i));
            set(gca,'XScale','log');
            set(gca,'YScale','log');
            xlim([1e-3 1e7]);
            ylim([ymin ymax]);
            text(1e1,ymin*6,'Time [d]');
            text(3e3,ymin*6,'Burnup [MWd/kg]');
            text(1e1,ymin*4,{ct(i,1)});
            text(3e3,ymin*4,{ct(i,2)});
            set(gca,'xtick',[1e-3 1e-2 1e-1 1 1e1 1e2 1e3 1e4 1e5 1e6 1e7]);
            frame2(i)=getframe;
        end
        set(figure(5),'Name','Flux Spectrum in Annular Region');
    end
    
    npts=length(a1(1,:));
    nact=0;
    act_print=[];
    'Charge / Discharge Actinide Compositions [w/o of IHM]:'
    for(i=1:length(act_names))
        nact=nact+a1(new_order(i),npts);
        if(plot_act(i))
          figure(i+5);
          hold on;
          if(y_axis_option)
            maxy=max(a2(new_order(i),:));
            plot(fine_d,a2(new_order(i),:)','LineWidth',2);     
          else
            maxy=max(a1(new_order(i),:));
            plot(fine_d,a1(new_order(i),:)','LineWidth',2);
          end
          if(maxy > 0) 
              ylim([0 maxy*1.1]);
          end
      end
      [act_out,err]=sprintf('%s    %7.4g    %7.4g\n',char(act_names(i)),a2(new_order(i),1), a2(new_order(i),npts));
      act_print=horzcat(act_print,act_out);
    end
    act_print
    'Total Transuranics [w/o of IHM]:'
    totalTRU=0;
    for i=10:24
        totalTRU=totalTRU+a2(new_order(i),npts);
    end
    totalTRU
    
    for i=1:100
        if(plot_options(4))
          figure(4);
          movie(frame,8,8);
        end
        if(plot_options(5))
          figure(5);
          movie(frame2,8,8);
        end
    end
    end
end

compp = comp';
rhop = rho';
vfp = vf';
save outputdeck;
