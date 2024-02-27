function varargout = VBUDSgui(varargin)
% VBUDSGUI M-file for VBUDSgui.fig
%      VBUDSGUI, by itself, creates a new VBUDSGUI or raises the existing
%      singleton*.
%
%      H = VBUDSGUI returns the handle to a new VBUDSGUI or the handle to
%      the existing singleton*.
%
%      VBUDSGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in VBUDSGUI.M with the given input arguments.
%
%      VBUDSGUI('Property','Value',...) creates a new VBUDSGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before VBUDSgui_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to VBUDSgui_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help VBUDSgui

% Last Modified by GUIDE v2.5 04-Dec-2004 16:34:49

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @VBUDSgui_OpeningFcn, ...
                   'gui_OutputFcn',  @VBUDSgui_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin & isstr(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT

% --- Executes just before VBUDSgui is made visible.
function VBUDSgui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to VBUDSgui (see VARARGIN)

% Choose default command line output for VBUDSgui
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);
load_case=varargin(1);
load_case=cell2mat(load_case);
default_to=0;
% UIWAIT makes VBUDSgui wait for user response (see UIRESUME)
% uiwait(handles.figure1);
if(load_case > 0) 
    set(handles.edit1,'String',horzcat('Saved ',num2str(load_case)));
end
set(handles.radiobutton1,'Value',1);
set(handles.radiobutton2,'Value',0);
set(handles.radiobutton5,'Value',1);
set(handles.radiobutton6,'Value',0);
set(handles.checkbox2,'Value',0);
set(handles.checkbox3,'Value',0);
set(handles.edit8,'String',0);
set(handles.edit9,'String',0);
set(handles.edit8,'Enable','off');
set(handles.edit9,'Enable','off');
set(handles.edit10,'String',0);
set(handles.edit11,'String',0);
set(handles.edit10,'Enable','off');
set(handles.edit11,'Enable','off');
set(handles.edit12,'String',0);
set(handles.edit13,'String',0);
set(handles.edit12,'Enable','off');
set(handles.edit13,'Enable','off');
set(handles.edit14,'String',0);
set(handles.edit15,'String',0);
set(handles.edit14,'Enable','off');
set(handles.edit15,'Enable','off');
set(handles.edit16,'String',95);
set(handles.edit17,'String',0.97);
set(handles.edit16,'Enable','on');
set(handles.edit17,'Enable','on');
set(handles.popupmenu7,'Value',2);
set(handles.edit18,'String',0);
set(handles.edit19,'String',0);
set(handles.edit18,'Enable','off');
set(handles.edit19,'Enable','off');
set(handles.edit20,'String',0);
set(handles.edit21,'String',0);
set(handles.edit20,'Enable','off');
set(handles.edit21,'Enable','off');
set(handles.edit22,'String',0);
set(handles.edit23,'String',0);
set(handles.edit22,'Enable','off');
set(handles.edit23,'Enable','off');
set(handles.edit24,'String',0);
set(handles.edit25,'String',0);
set(handles.edit26,'String',0);
set(handles.edit27,'String',0);
set(handles.edit28,'String',0);
set(handles.edit29,'String',0);
set(handles.edit30,'String',3.5);
set(handles.edit31,'String',0);
set(handles.edit32,'String',96.5);
set(handles.edit33,'String',0);
set(handles.edit34,'String',0);
set(handles.edit35,'String',0);
set(handles.edit36,'String',0);
set(handles.edit37,'String',0);
set(handles.edit38,'String',0);
set(handles.edit39,'String',0);
set(handles.edit40,'String',0);
set(handles.edit41,'String',0);
set(handles.edit42,'String',0);
set(handles.edit43,'String',0);
set(handles.edit44,'String',0);
set(handles.edit45,'String',0);
set(handles.edit46,'String',0);
set(handles.edit47,'String',0);
set(handles.edit48,'String',0.5);
set(handles.edit49,'String',1.4);
set(handles.edit50,'String',0.05);
set(handles.edit51,'String','N/A');
set(handles.edit51,'Enable','off');
set(handles.edit52,'String','N/A');
set(handles.edit52,'Enable','off');
set(handles.popupmenu12,'Value',2);
set(handles.popupmenu13,'Value',2);
set(handles.edit53,'String',5.87);
set(handles.edit54,'String',7.7);
set(handles.edit55,'String',2.5);
set(handles.edit56,'String',300);
set(handles.edit58,'String',300);
set(handles.edit59,'String',1e6);
set(handles.edit60,'String',1e6);
shut_down_shim(handles);
vf0=100.;
if(get(handles.radiobutton5,'Value')) 
    vf0 = vf0-str2double(get(handles.edit4,'String'));
end
set(handles.edit5,'String',vf0);
vf1=100.;
if(get(handles.radiobutton6,'Value')) 
    vf1 = vf1-str2double(get(handles.edit4,'String'));
end
vf1 = vf1 - str2double(get(handles.edit16,'String'));
set(handles.edit6,'String',vf1);
if(load_case > 0)
    case_data = get_case(load_case);
    set(handles.radiobutton5,'Value',abs(2-case_data(1)));
    set(handles.radiobutton6,'Value',abs(1-case_data(1)));
    set(handles.popupmenu1,'Value',case_data(2));
    set(handles.popupmenu2,'Value',case_data(3));
    if(case_data(4) > 0)
        set(handles.checkbox2,'Value',2-case_data(4));
        set(handles.checkbox3,'Value',abs(1-case_data(4)));
        set(handles.edit52,'Enable','on'); 
        set(handles.edit52,'String',case_data(5));    
    else
        set(handles.edit52,'Enable','off'); 
        set(handles.edit52,'String','N/A');      
    end
    set(handles.popupmenu11,'Value',case_data(6));
    set(handles.edit3,'String',case_data(7));
    set(handles.edit4,'String',case_data(8));
    set(handles.popupmenu3,'Value',case_data(9));
    if(case_data(9) > 1)
        set(handles.edit8,'String',case_data(11));
        set(handles.edit9,'String',case_data(10));
        set(handles.edit8,'Enable','on');
        set(handles.edit9,'Enable','on');         
    else
        set(handles.edit8,'String',0);
        set(handles.edit9,'String',0);        
        set(handles.edit8,'Enable','off');
        set(handles.edit9,'Enable','off');        
    end
    set(handles.popupmenu4,'Value',case_data(12));
    if(case_data(12) > 1)
        set(handles.edit10,'String',case_data(14));
        set(handles.edit11,'String',case_data(13));
        set(handles.edit10,'Enable','on');
        set(handles.edit11,'Enable','on');         
    else
        set(handles.edit10,'String',0);
        set(handles.edit11,'String',0);        
        set(handles.edit10,'Enable','off');
        set(handles.edit11,'Enable','off');        
    end
    set(handles.popupmenu5,'Value',case_data(15));
    if(case_data(15) > 1)
        set(handles.edit12,'String',case_data(17));
        set(handles.edit13,'String',case_data(16));
        set(handles.edit12,'Enable','on');
        set(handles.edit13,'Enable','on');         
    else
        set(handles.edit12,'String',0);
        set(handles.edit13,'String',0);        
        set(handles.edit12,'Enable','off');
        set(handles.edit13,'Enable','off');        
    end
    set(handles.popupmenu6,'Value',case_data(18));
    if(case_data(18) > 1)
        set(handles.edit14,'String',case_data(20));
        set(handles.edit15,'String',case_data(19));
        set(handles.edit14,'Enable','on');
        set(handles.edit15,'Enable','on');         
    else
        set(handles.edit14,'String',0);
        set(handles.edit15,'String',0);        
        set(handles.edit14,'Enable','off');
        set(handles.edit15,'Enable','off');        
    end
    set(handles.popupmenu7,'Value',case_data(21));
    if(case_data(21) > 1)
        set(handles.edit16,'String',case_data(23));
        set(handles.edit17,'String',case_data(22));
        set(handles.edit16,'Enable','on');
        set(handles.edit17,'Enable','on');         
    else
        set(handles.edit16,'String',0);
        set(handles.edit17,'String',0);        
        set(handles.edit16,'Enable','off');
        set(handles.edit17,'Enable','off');        
    end
    set(handles.popupmenu8,'Value',case_data(24));
    if(case_data(24) > 1)
        set(handles.edit18,'String',case_data(26));
        set(handles.edit19,'String',case_data(25));
        set(handles.edit18,'Enable','on');
        set(handles.edit19,'Enable','on');         
    else
        set(handles.edit18,'String',0);
        set(handles.edit19,'String',0);        
        set(handles.edit18,'Enable','off');
        set(handles.edit19,'Enable','off');        
    end
    set(handles.popupmenu9,'Value',case_data(27));
    if(case_data(27) > 1)
        set(handles.edit20,'String',case_data(29));
        set(handles.edit21,'String',case_data(28));
        set(handles.edit20,'Enable','on');
        set(handles.edit21,'Enable','on');         
    else
        set(handles.edit20,'String',0);
        set(handles.edit21,'String',0);        
        set(handles.edit20,'Enable','off');
        set(handles.edit21,'Enable','off');        
    end
    set(handles.popupmenu10,'Value',case_data(30));
    if(case_data(30) > 1)
        set(handles.edit22,'String',case_data(32));
        set(handles.edit23,'String',case_data(31));
        set(handles.edit22,'Enable','on');
        set(handles.edit23,'Enable','on');         
    else
        set(handles.edit22,'String',0);
        set(handles.edit23,'String',0);        
        set(handles.edit22,'Enable','off');
        set(handles.edit23,'Enable','off');        
    end
    set(handles.radiobutton1,'Value',2-case_data(33));
    set(handles.radiobutton2,'Value',abs(1-case_data(33)));
    set(handles.edit24,'String',case_data(34));
    set(handles.edit25,'String',case_data(35));
    set(handles.edit26,'String',case_data(36));
    set(handles.edit27,'String',case_data(37));
    set(handles.edit28,'String',case_data(38));
    set(handles.edit29,'String',case_data(39));
    set(handles.edit30,'String',case_data(40));
    set(handles.edit31,'String',case_data(41));
    set(handles.edit32,'String',case_data(42));
    set(handles.edit33,'String',case_data(43));
    set(handles.edit34,'String',case_data(44));
    set(handles.edit35,'String',case_data(45));
    set(handles.edit36,'String',case_data(46));
    set(handles.edit37,'String',case_data(47));
    set(handles.edit38,'String',case_data(48));
    set(handles.edit39,'String',case_data(49));
    set(handles.edit40,'String',case_data(50));
    set(handles.edit41,'String',case_data(51));
    set(handles.edit42,'String',case_data(52));
    set(handles.edit43,'String',case_data(53));
    set(handles.edit44,'String',case_data(54));
    set(handles.edit45,'String',case_data(55));
    set(handles.edit46,'String',case_data(56));
    set(handles.edit47,'String',case_data(57));
    set(handles.popupmenu12,'Value',case_data(58));
    if(case_data(58) > 1)
        set(handles.edit53,'String',case_data(59));
        set(handles.edit53,'Enable','on');    
    else
        set(handles.edit53,'String',0);
        set(handles.edit53,'Enable','off');
    end
    set(handles.popupmenu13,'Value',case_data(60));
    if(case_data(60) > 1)
        set(handles.edit54,'String',case_data(61));
        set(handles.edit55,'String',case_data(62));        
        set(handles.edit54,'Enable','on');     
        set(handles.edit55,'Enable','on');
    else
        set(handles.edit54,'String',0);
        set(handles.edit54,'Enable','off');  
        set(handles.edit55,'String',0);
        set(handles.edit55,'Enable','off');
    end
    set(handles.edit56,'String',case_data(63));
    set(handles.edit58,'String',case_data(64));
    set(handles.edit59,'String',case_data(65));
    set(handles.edit60,'String',case_data(66));
    set(handles.edit48,'String',case_data(67));
    set(handles.edit49,'String',case_data(69));
    set(handles.edit50,'String',case_data(68));   
    edit5_Callback(handles.edit5,eventdata,handles);
    edit6_Callback(handles.edit6,eventdata,handles);
end
if(default_to==1)
    set(handles.edit24,'String',0);
    set(handles.edit25,'String',0);
    set(handles.edit26,'String',0);
    set(handles.edit27,'String',0);
    set(handles.edit28,'String',0);
    set(handles.edit29,'String',0);
    set(handles.edit30,'String',0);
    set(handles.edit31,'String',0);
    set(handles.edit32,'String',0);
    set(handles.edit33,'String',.113);
    set(handles.edit34,'String',0);
    set(handles.edit35,'String',.0332);
    set(handles.edit36,'String',.925);
    set(handles.edit37,'String',.412);
    set(handles.edit38,'String',.0627);
    set(handles.edit39,'String',.113);
    set(handles.edit40,'String',.205);
    set(handles.edit41,'String',1.68e-4);
    set(handles.edit42,'String',2.53e-2);
    set(handles.edit43,'String',4.06e-7);
    set(handles.edit44,'String',5.95e-5);
    set(handles.edit45,'String',2.90e-3);
    set(handles.edit46,'String',4.08e-4);
    set(handles.edit47,'String',5.49e-5);
    set(handles.edit48,'String',0.4096);
    set(handles.edit49,'String',1.27);
    set(handles.edit50,'String',0.065);
    set(handles.edit55,'String',.001);
    set(handles.edit56,'String',800);
    set(handles.edit58,'String',550);
    set(handles.edit59,'String',1e6);
    set(handles.edit60,'String',1e6);
end
if(default_to==2)
    set(handles.radiobutton1,'Value',0);
    set(handles.radiobutton2,'Value',1);
    set(handles.edit24,'String',0);
    set(handles.edit25,'String',0);
    set(handles.edit26,'String',0);
    set(handles.edit27,'String',0);
    set(handles.edit28,'String',0);
    set(handles.edit29,'String',2.60e-7);
    set(handles.edit30,'String',5.43e-5);
    set(handles.edit31,'String',0);
    set(handles.edit32,'String',2.14e-2);
    set(handles.edit33,'String',0);
    set(handles.edit34,'String',0);
    set(handles.edit35,'String',4.66e-5);
    set(handles.edit36,'String',1.02e-3);
    set(handles.edit37,'String',4.83e-4);
    set(handles.edit38,'String',1.75e-4);
    set(handles.edit39,'String',1.32e-4);
    set(handles.edit40,'String',0);
    set(handles.edit41,'String',0);
    set(handles.edit42,'String',0);
    set(handles.edit43,'String',0);
    set(handles.edit44,'String',0);
    set(handles.edit45,'String',0);
    set(handles.edit46,'String',0);
    set(handles.edit47,'String',0);
end
if(default_to==3)
    set(handles.radiobutton1,'Value',0);
    set(handles.radiobutton2,'Value',1);
    set(handles.edit24,'String',0);
    set(handles.edit25,'String',0);
    set(handles.edit26,'String',0);
    set(handles.edit27,'String',0);
    set(handles.edit28,'String',0);
    set(handles.edit29,'String',2.780e-7);
    set(handles.edit30,'String',5.857e-5);
    set(handles.edit31,'String',0);
    set(handles.edit32,'String',2.307e-2);
    set(handles.edit33,'String',0);
    set(handles.edit34,'String',0);
    set(handles.edit35,'String',2.470e-5);
    set(handles.edit36,'String',8.062e-4);
    set(handles.edit37,'String',3.130e-4);
    set(handles.edit38,'String',1.653e-4);
    set(handles.edit39,'String',5.398e-5);
    set(handles.edit40,'String',0);
    set(handles.edit41,'String',0);
    set(handles.edit42,'String',0);
    set(handles.edit43,'String',0);
    set(handles.edit44,'String',0);
    set(handles.edit45,'String',0);
    set(handles.edit46,'String',0);
    set(handles.edit47,'String',0);
    set(handles.edit48,'String',0.412);
    set(handles.edit49,'String',1.33);
    set(handles.edit50,'String',0.063);
    set(handles.edit16,'String',100);
    set(handles.edit17,'String',0.997);
    set(handles.edit55,'String',0.0);
end
if(default_to==4)
    set(handles.radiobutton1,'Value',0);
    set(handles.radiobutton2,'Value',1);
    set(handles.edit24,'String',0);
    set(handles.edit25,'String',0);
    set(handles.edit26,'String',0);
    set(handles.edit27,'String',0);
    set(handles.edit28,'String',0);
    set(handles.edit29,'String',2.595e-7);
    set(handles.edit30,'String',5.429e-5);
    set(handles.edit31,'String',0);
    set(handles.edit32,'String',2.139e-2);
    set(handles.edit33,'String',0);
    set(handles.edit34,'String',0);
    set(handles.edit35,'String',4.661e-5);
    set(handles.edit36,'String',1.016e-3);
    set(handles.edit37,'String',4.826e-4);
    set(handles.edit38,'String',1.749e-4);
    set(handles.edit39,'String',1.320e-4);
    set(handles.edit40,'String',0);
    set(handles.edit41,'String',0);
    set(handles.edit42,'String',0);
    set(handles.edit43,'String',0);
    set(handles.edit44,'String',0);
    set(handles.edit45,'String',0);
    set(handles.edit46,'String',0);
    set(handles.edit47,'String',0);
    set(handles.edit48,'String',0.410);
    set(handles.edit49,'String',1.3127);
    set(handles.edit50,'String',0.065);
    set(handles.edit16,'String',100);
    set(handles.edit17,'String',0.724);
    set(handles.edit55,'String',0.0);
    set(handles.edit56,'String',900.0001);
    set(handles.edit58,'String',575);
    set(handles.edit4,'String',97.5);
    set(handles.checkbox3,'Value',1);
    wake_up_shim(handles);
    set(handles.edit52,'String',1000);
end
plot_geometry(handles);
get_nd(handles);


% --- Outputs from this function are returned to the command line.
function varargout = VBUDSgui_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes during object creation, after setting all properties.
function edit1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


function edit1_Callback(hObject, eventdata, handles)
% hObject    handle to edit1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit1 as text
%        str2double(get(hObject,'String')) returns contents of edit1 as a double


% --- Executes on button press in radiobutton1.
function radiobutton1_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton1
set(handles.radiobutton2,'Value',0);
reinitialize_actinides(handles);

% --- Executes on button press in radiobutton2.
function radiobutton2_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton2
set(handles.radiobutton1,'Value',0);
reinitialize_actinides(handles);

% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
set(handles.pushbutton1,'String','Wait...');


% --- Executes during object creation, after setting all properties.
function popupmenu1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu1.
function popupmenu1_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu1 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu1


% --- Executes on button press in radiobutton5.
function radiobutton5_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton5
set(handles.radiobutton6,'Value',0);
edit5_Callback(handles.edit5,eventdata,handles);
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes on button press in radiobutton6.
function radiobutton6_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton6
set(handles.radiobutton5,'Value',0);
edit5_Callback(handles.edit5,eventdata,handles);
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function popupmenu2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu2.
function popupmenu2_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu2 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu2


% --- Executes during object creation, after setting all properties.
function edit3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit3_Callback(hObject, eventdata, handles)
% hObject    handle to edit3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit3 as text
%        str2double(get(hObject,'String')) returns contents of edit3 as a double


% --- Executes during object creation, after setting all properties.
function edit4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit4_Callback(hObject, eventdata, handles)
% hObject    handle to edit4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit4 as text
%        str2double(get(hObject,'String')) returns contents of edit4 as a double
if(get(handles.radiobutton5,'Value')) 
    edit5_Callback(handles.edit5,eventdata,handles);
else 
    edit6_Callback(handles.edit6,eventdata,handles);
end


% --- Executes on button press in checkbox2.
function checkbox2_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox2
set(handles.checkbox3,'Value',0);
if(get(hObject,'Value'))
    wake_up_shim(handles);
else
    if(get(hObject,'Value')==0 && get(handles.checkbox3,'Value')==0)
        shut_down_shim(handles);
    end
end

% --- Executes on button press in checkbox3.
function checkbox3_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox3
set(handles.checkbox2,'Value',0);
if(get(hObject,'Value'))
    wake_up_shim(handles);
else
    if(get(hObject,'Value')==0 && get(handles.checkbox2,'Value')==0)
        shut_down_shim(handles);
    end
end

% --- Executes during object creation, after setting all properties.
function edit5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end

function edit5_Callback(hObject, eventdata, handles)
% hObject    handle to edit5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit5 as text
%        str2double(get(hObject,'String')) returns contents of edit5 as a double
vf = 100.;
if(get(handles.radiobutton5,'Value')) 
    vf = vf-str2double(get(handles.edit4,'String'));
end
vf = vf-str2double(get(handles.edit8,'String'));
vf = vf-str2double(get(handles.edit10,'String'));
vf = vf-str2double(get(handles.edit12,'String'));
vf = vf-str2double(get(handles.edit14,'String'));
set(handles.edit5,'String',vf);


% --- Executes during object creation, after setting all properties.
function edit6_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit6_Callback(hObject, eventdata, handles)
% hObject    handle to edit6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit6 as text
%        str2double(get(hObject,'String')) returns contents of edit6 as a double
vf = 100.;
if(get(handles.radiobutton6,'Value')) 
    vf = vf-str2double(get(handles.edit4,'String'));
end
vf = vf-str2double(get(handles.edit16,'String'));
vf = vf-str2double(get(handles.edit18,'String'));
vf = vf-str2double(get(handles.edit20,'String'));
vf = vf-str2double(get(handles.edit22,'String'));
set(handles.edit6,'String',vf);


% --- Executes during object creation, after setting all properties.
function popupmenu3_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu3.
function popupmenu3_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu3 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu3
if(get(hObject,'Value')<2)
    set(handles.edit8,'String',0);
    set(handles.edit9,'String',0);
    set(handles.edit8,'Enable','off');
    set(handles.edit9,'Enable','off');
else
    set(handles.edit8,'String',0);
    set(handles.edit9,'String',0);
    set(handles.edit8,'Enable','on');
    set(handles.edit9,'Enable','on');
end
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit8_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit8_Callback(hObject, eventdata, handles)
% hObject    handle to edit8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit8 as text
%        str2double(get(hObject,'String')) returns contents of edit8 as a double
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit9_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit9_Callback(hObject, eventdata, handles)
% hObject    handle to edit9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit9 as text
%        str2double(get(hObject,'String')) returns contents of edit9 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu4_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu4.
function popupmenu4_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu4 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu4
if(get(hObject,'Value')<2)
    set(handles.edit10,'String',0);
    set(handles.edit11,'String',0);
    set(handles.edit10,'Enable','off');
    set(handles.edit11,'Enable','off');
else
    set(handles.edit10,'String',0);
    set(handles.edit11,'String',0);
    set(handles.edit10,'Enable','on');
    set(handles.edit11,'Enable','on');
end
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit10_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit10_Callback(hObject, eventdata, handles)
% hObject    handle to edit10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit10 as text
%        str2double(get(hObject,'String')) returns contents of edit10 as a double
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit11_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit11_Callback(hObject, eventdata, handles)
% hObject    handle to edit11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit11 as text
%        str2double(get(hObject,'String')) returns contents of edit11 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu5_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu5.
function popupmenu5_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu5 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu5
if(get(hObject,'Value')<2)
    set(handles.edit12,'String',0);
    set(handles.edit13,'String',0);
    set(handles.edit12,'Enable','off');
    set(handles.edit13,'Enable','off');
else
    set(handles.edit12,'String',0);
    set(handles.edit13,'String',0);
    set(handles.edit12,'Enable','on');
    set(handles.edit13,'Enable','on');
end
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit12_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit12_Callback(hObject, eventdata, handles)
% hObject    handle to edit12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit12 as text
%        str2double(get(hObject,'String')) returns contents of edit12 as a double
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit13_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit13_Callback(hObject, eventdata, handles)
% hObject    handle to edit13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit13 as text
%        str2double(get(hObject,'String')) returns contents of edit13 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu6_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu6.
function popupmenu6_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu6 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu6
if(get(hObject,'Value')<2)
    set(handles.edit14,'String',0);
    set(handles.edit15,'String',0);
    set(handles.edit14,'Enable','off');
    set(handles.edit15,'Enable','off');
else
    set(handles.edit14,'String',0);
    set(handles.edit15,'String',0);
    set(handles.edit14,'Enable','on');
    set(handles.edit15,'Enable','on');
end
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit14_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit14 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit14_Callback(hObject, eventdata, handles)
% hObject    handle to edit14 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit14 as text
%        str2double(get(hObject,'String')) returns contents of edit14 as a double
edit5_Callback(handles.edit5,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit15_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit15 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit15_Callback(hObject, eventdata, handles)
% hObject    handle to edit15 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit15 as text
%        str2double(get(hObject,'String')) returns contents of edit15 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu7_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu7.
function popupmenu7_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu7 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu7
if(get(hObject,'Value')<2)
    set(handles.edit16,'String',0);
    set(handles.edit17,'String',0);
    set(handles.edit16,'Enable','off');
    set(handles.edit17,'Enable','off');
else
    set(handles.edit16,'String',0);
    set(handles.edit17,'String',0);
    set(handles.edit16,'Enable','on');
    set(handles.edit17,'Enable','on');
end
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit16_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit16 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit16_Callback(hObject, eventdata, handles)
% hObject    handle to edit16 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit16 as text
%        str2double(get(hObject,'String')) returns contents of edit16 as a double
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit17_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit17 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit17_Callback(hObject, eventdata, handles)
% hObject    handle to edit17 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit17 as text
%        str2double(get(hObject,'String')) returns contents of edit17 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu8_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu8.
function popupmenu8_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu8 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu8
if(get(hObject,'Value')<2)
    set(handles.edit18,'String',0);
    set(handles.edit19,'String',0);
    set(handles.edit18,'Enable','off');
    set(handles.edit19,'Enable','off');
else
    set(handles.edit18,'String',0);
    set(handles.edit19,'String',0);
    set(handles.edit18,'Enable','on');
    set(handles.edit19,'Enable','on');
end
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit18_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit18 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit18_Callback(hObject, eventdata, handles)
% hObject    handle to edit18 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit18 as text
%        str2double(get(hObject,'String')) returns contents of edit18 as a double
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit19_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit19 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit19_Callback(hObject, eventdata, handles)
% hObject    handle to edit19 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit19 as text
%        str2double(get(hObject,'String')) returns contents of edit19 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu9_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu9.
function popupmenu9_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu9 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu9
if(get(hObject,'Value')<2)
    set(handles.edit20,'String',0);
    set(handles.edit21,'String',0);
    set(handles.edit20,'Enable','off');
    set(handles.edit21,'Enable','off');
else
    set(handles.edit20,'String',0);
    set(handles.edit21,'String',0);
    set(handles.edit20,'Enable','on');
    set(handles.edit21,'Enable','on');
end
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit20_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit20 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit20_Callback(hObject, eventdata, handles)
% hObject    handle to edit20 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit20 as text
%        str2double(get(hObject,'String')) returns contents of edit20 as a double
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit21_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit21 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit21_Callback(hObject, eventdata, handles)
% hObject    handle to edit21 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit21 as text
%        str2double(get(hObject,'String')) returns contents of edit21 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu10_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu10.
function popupmenu10_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu10 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu10
if(get(hObject,'Value')<2)
    set(handles.edit22,'String',0);
    set(handles.edit23,'String',0);
    set(handles.edit22,'Enable','off');
    set(handles.edit23,'Enable','off');
else
    set(handles.edit22,'String',0);
    set(handles.edit23,'String',0);
    set(handles.edit22,'Enable','on');
    set(handles.edit23,'Enable','on');
end
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit22_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit22 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit22_Callback(hObject, eventdata, handles)
% hObject    handle to edit22 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit22 as text
%        str2double(get(hObject,'String')) returns contents of edit22 as a double
edit6_Callback(handles.edit6,eventdata,handles);

% --- Executes during object creation, after setting all properties.
function edit23_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit23 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit23_Callback(hObject, eventdata, handles)
% hObject    handle to edit23 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit23 as text
%        str2double(get(hObject,'String')) returns contents of edit23 as a double


% --- Executes during object creation, after setting all properties.
function edit24_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit24 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit24_Callback(hObject, eventdata, handles)
% hObject    handle to edit24 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit24 as text
%        str2double(get(hObject,'String')) returns contents of edit24 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit25_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit25 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit25_Callback(hObject, eventdata, handles)
% hObject    handle to edit25 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit25 as text
%        str2double(get(hObject,'String')) returns contents of edit25 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit26_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit26 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit26_Callback(hObject, eventdata, handles)
% hObject    handle to edit26 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit26 as text
%        str2double(get(hObject,'String')) returns contents of edit26 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit27_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit27 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit27_Callback(hObject, eventdata, handles)
% hObject    handle to edit27 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit27 as text
%        str2double(get(hObject,'String')) returns contents of edit27 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit28_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit28 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit28_Callback(hObject, eventdata, handles)
% hObject    handle to edit28 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit28 as text
%        str2double(get(hObject,'String')) returns contents of edit28 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit29_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit29 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit29_Callback(hObject, eventdata, handles)
% hObject    handle to edit29 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit29 as text
%        str2double(get(hObject,'String')) returns contents of edit29 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit30_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit30 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit30_Callback(hObject, eventdata, handles)
% hObject    handle to edit30 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit30 as text
%        str2double(get(hObject,'String')) returns contents of edit30 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit31_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit31 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit31_Callback(hObject, eventdata, handles)
% hObject    handle to edit31 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit31 as text
%        str2double(get(hObject,'String')) returns contents of edit31 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit32_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit32 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit32_Callback(hObject, eventdata, handles)
% hObject    handle to edit32 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit32 as text
%        str2double(get(hObject,'String')) returns contents of edit32 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit33_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit33 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit33_Callback(hObject, eventdata, handles)
% hObject    handle to edit33 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit33 as text
%        str2double(get(hObject,'String')) returns contents of edit33 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit34_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit34 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit34_Callback(hObject, eventdata, handles)
% hObject    handle to edit34 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit34 as text
%        str2double(get(hObject,'String')) returns contents of edit34 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit35_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit35 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit35_Callback(hObject, eventdata, handles)
% hObject    handle to edit35 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit35 as text
%        str2double(get(hObject,'String')) returns contents of edit35 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit36_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit36 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit36_Callback(hObject, eventdata, handles)
% hObject    handle to edit36 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit36 as text
%        str2double(get(hObject,'String')) returns contents of edit36 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit37_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit37 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit37_Callback(hObject, eventdata, handles)
% hObject    handle to edit37 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit37 as text
%        str2double(get(hObject,'String')) returns contents of edit37 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit38_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit38 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit38_Callback(hObject, eventdata, handles)
% hObject    handle to edit38 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit38 as text
%        str2double(get(hObject,'String')) returns contents of edit38 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit39_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit39 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit39_Callback(hObject, eventdata, handles)
% hObject    handle to edit39 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit39 as text
%        str2double(get(hObject,'String')) returns contents of edit39 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit40_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit40 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit40_Callback(hObject, eventdata, handles)
% hObject    handle to edit40 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit40 as text
%        str2double(get(hObject,'String')) returns contents of edit40 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit41_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit41 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit41_Callback(hObject, eventdata, handles)
% hObject    handle to edit41 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit41 as text
%        str2double(get(hObject,'String')) returns contents of edit41 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit42_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit42 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit42_Callback(hObject, eventdata, handles)
% hObject    handle to edit42 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit42 as text
%        str2double(get(hObject,'String')) returns contents of edit42 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit43_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit43 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit43_Callback(hObject, eventdata, handles)
% hObject    handle to edit43 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit43 as text
%        str2double(get(hObject,'String')) returns contents of edit43 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit44_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit44 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit44_Callback(hObject, eventdata, handles)
% hObject    handle to edit44 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit44 as text
%        str2double(get(hObject,'String')) returns contents of edit44 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit45_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit45 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit45_Callback(hObject, eventdata, handles)
% hObject    handle to edit45 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit45 as text
%        str2double(get(hObject,'String')) returns contents of edit45 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit46_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit46 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit46_Callback(hObject, eventdata, handles)
% hObject    handle to edit46 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit46 as text
%        str2double(get(hObject,'String')) returns contents of edit46 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit47_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit47 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit47_Callback(hObject, eventdata, handles)
% hObject    handle to edit47 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit47 as text
%        str2double(get(hObject,'String')) returns contents of edit47 as a double
get_nd(handles);

% --- Executes during object creation, after setting all properties.
function edit48_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit48 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit48_Callback(hObject, eventdata, handles)
% hObject    handle to edit48 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit48 as text
%        str2double(get(hObject,'String')) returns contents of edit48 as a double
plot_geometry(handles);

% --- Executes during object creation, after setting all properties.
function edit49_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit49 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit49_Callback(hObject, eventdata, handles)
% hObject    handle to edit49 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit49 as text
%        str2double(get(hObject,'String')) returns contents of edit49 as a double
plot_geometry(handles);

% --- Executes during object creation, after setting all properties.
function edit50_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit50 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit50_Callback(hObject, eventdata, handles)
% hObject    handle to edit50 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit50 as text
%        str2double(get(hObject,'String')) returns contents of edit50 as a double
plot_geometry(handles);

function plot_geometry(handles)

r = str2double(get(handles.edit48,'String'));
p = str2double(get(handles.edit49,'String'));
t = str2double(get(handles.edit50,'String'));
x = linspace(-r,r,200);
y = sqrt(r^2.-x.^2.);

axes(handles.axes1);

hold off;
plot(x,y,'Color',[.8 0 .2]);
hold on;
plot(x,-y,'Color',[.8 0 .2]);

ro = r+t;
xo = linspace(-ro, ro,200);
yo = sqrt(ro^2.-xo.^2.);
plot(xo,yo,'Color',[.2 .8 0]);
plot(xo,-yo,'Color',[.2 .8 0]);

plot([-p/2 p/2 p/2 -p/2 -p/2],[-p/2 -p/2 p/2 p/2 -p/2],'Color',[.0 .2 .8]);
xlabel('[cm]');
ylabel('[cm]');
axis square;
grid on;

function get_nd(handles)

if(get(handles.radiobutton2,'Value'))
total=0.;
total=total+str2double(get(handles.edit24,'String'));
total=total+str2double(get(handles.edit25,'String'));
total=total+str2double(get(handles.edit26,'String'));
total=total+str2double(get(handles.edit27,'String'));
total=total+str2double(get(handles.edit28,'String'));
total=total+str2double(get(handles.edit29,'String'));
total=total+str2double(get(handles.edit30,'String'));
total=total+str2double(get(handles.edit31,'String'));
total=total+str2double(get(handles.edit32,'String'));
total=total+str2double(get(handles.edit33,'String'));
total=total+str2double(get(handles.edit34,'String'));
total=total+str2double(get(handles.edit35,'String'));
total=total+str2double(get(handles.edit36,'String'));
total=total+str2double(get(handles.edit37,'String'));
total=total+str2double(get(handles.edit38,'String'));
total=total+str2double(get(handles.edit39,'String'));
total=total+str2double(get(handles.edit40,'String'));
total=total+str2double(get(handles.edit41,'String'));
total=total+str2double(get(handles.edit42,'String'));
total=total+str2double(get(handles.edit43,'String'));
total=total+str2double(get(handles.edit44,'String'));
total=total+str2double(get(handles.edit45,'String'));
total=total+str2double(get(handles.edit46,'String'));
total=total+str2double(get(handles.edit47,'String'));

set(handles.edit51,'String',total);
else
    set(handles.edit51,'String','N/A');
end

function reinitialize_actinides(handles)

default_to=round(get(handles.text1,'Value'));
if(get(handles.radiobutton2,'Value'))
    if(default_to==2)
set(handles.edit24,'String',0);
set(handles.edit25,'String',0);
set(handles.edit26,'String',0);
set(handles.edit27,'String',0);
set(handles.edit28,'String',0);
set(handles.edit29,'String',2.60e-7);
set(handles.edit30,'String',5.43e-5);
set(handles.edit31,'String',0);
set(handles.edit32,'String',2.14e-2);
set(handles.edit33,'String',0);
set(handles.edit34,'String',0);
set(handles.edit35,'String',4.66e-5);
set(handles.edit36,'String',1.02e-3);
set(handles.edit37,'String',4.83e-4);
set(handles.edit38,'String',1.75e-4);
set(handles.edit39,'String',1.32e-4);
set(handles.edit40,'String',0);
set(handles.edit41,'String',0);
set(handles.edit42,'String',0);
set(handles.edit43,'String',0);
set(handles.edit44,'String',0);
set(handles.edit45,'String',0);
set(handles.edit46,'String',0);
set(handles.edit47,'String',0);
    else
set(handles.edit24,'String',0);
set(handles.edit25,'String',0);
set(handles.edit26,'String',0);
set(handles.edit27,'String',0);
set(handles.edit28,'String',0);
set(handles.edit29,'String',0);
set(handles.edit30,'String',0);
set(handles.edit31,'String',0);
set(handles.edit32,'String',0);
set(handles.edit33,'String',0);
set(handles.edit34,'String',0);
set(handles.edit35,'String',0);
set(handles.edit36,'String',0);
set(handles.edit37,'String',0);
set(handles.edit38,'String',0);
set(handles.edit39,'String',0);
set(handles.edit40,'String',0);
set(handles.edit41,'String',0);
set(handles.edit42,'String',0);
set(handles.edit43,'String',0);
set(handles.edit44,'String',0);
set(handles.edit45,'String',0);
set(handles.edit46,'String',0);
set(handles.edit47,'String',0); 
    end
else
    if(default_to==1)
    set(handles.edit24,'String',0);
    set(handles.edit25,'String',0);
    set(handles.edit26,'String',0);
    set(handles.edit27,'String',0);
    set(handles.edit28,'String',0);
    set(handles.edit29,'String',0);
    set(handles.edit30,'String',0);
    set(handles.edit31,'String',0);
    set(handles.edit32,'String',0);
    set(handles.edit33,'String',.113);
    set(handles.edit34,'String',0);
    set(handles.edit35,'String',.0332);
    set(handles.edit36,'String',.925);
    set(handles.edit37,'String',.412);
    set(handles.edit38,'String',.0627);
    set(handles.edit39,'String',.113);
    set(handles.edit40,'String',.205);
    set(handles.edit41,'String',1.68e-4);
    set(handles.edit42,'String',2.53e-2);
    set(handles.edit43,'String',4.06e-7);
    set(handles.edit44,'String',5.95e-5);
    set(handles.edit45,'String',2.90e-3);
    set(handles.edit46,'String',4.08e-4);
    set(handles.edit47,'String',5.49e-5);
        
    else
set(handles.edit24,'String',0);
set(handles.edit25,'String',0);
set(handles.edit26,'String',0);
set(handles.edit27,'String',0);
set(handles.edit28,'String',0);
set(handles.edit29,'String',0);
set(handles.edit30,'String',0);
set(handles.edit31,'String',0);
set(handles.edit32,'String',0);
set(handles.edit33,'String',0);
set(handles.edit34,'String',0);
set(handles.edit35,'String',0);
set(handles.edit36,'String',0);
set(handles.edit37,'String',0);
set(handles.edit38,'String',0);
set(handles.edit39,'String',0);
set(handles.edit40,'String',0);
set(handles.edit41,'String',0);
set(handles.edit42,'String',0);
set(handles.edit43,'String',0);
set(handles.edit44,'String',0);
set(handles.edit45,'String',0);
set(handles.edit46,'String',0);
set(handles.edit47,'String',0);  
    end
end
if(get(handles.radiobutton2,'Value'))
    get_nd(handles);
else
    set(handles.edit51,'String','N/A');
end

function wake_up_shim(handles)

set(handles.edit52,'Enable','on');
set(handles.edit52,'String',0);

function shut_down_shim(handles)

set(handles.edit52,'Enable','off');
set(handles.edit52,'String','N/A');

function activate_cladding(handles)

set(handles.edit53,'Enable','on');
set(handles.edit53,'String',5.87);

function deactivate_cladding(handles)

set(handles.edit53,'Enable','off');
set(handles.edit53,'String','N/A');

function activate_structural(handles)

set(handles.edit54,'Enable','on');
set(handles.edit55,'Enable','on');
set(handles.edit54,'String',7.7);
set(handles.edit55,'String',2.5);

function deactivate_structural(handles)

set(handles.edit54,'Enable','off');
set(handles.edit55,'Enable','off');
set(handles.edit54,'String','N/A');
set(handles.edit55,'String','N/A');

% --- Executes during object creation, after setting all properties.
function edit51_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit51 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit51_Callback(hObject, eventdata, handles)
% hObject    handle to edit51 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit51 as text
%        str2double(get(hObject,'String')) returns contents of edit51 as a double


% --- Executes during object creation, after setting all properties.
function edit52_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit52 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit52_Callback(hObject, eventdata, handles)
% hObject    handle to edit52 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit52 as text
%        str2double(get(hObject,'String')) returns contents of edit52 as a double


% --- Executes during object creation, after setting all properties.
function popupmenu11_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu11.
function popupmenu11_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu11 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu11


% --- Executes during object creation, after setting all properties.
function popupmenu12_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu12.
function popupmenu12_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu12 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu12
if(get(hObject,'Value')-1)
    activate_cladding(handles);
else
    deactivate_cladding(handles);
end

% --- Executes during object creation, after setting all properties.
function popupmenu13_CreateFcn(hObject, eventdata, handles)
% hObject    handle to popupmenu13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: popupmenu controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on selection change in popupmenu13.
function popupmenu13_Callback(hObject, eventdata, handles)
% hObject    handle to popupmenu13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: contents = get(hObject,'String') returns popupmenu13 contents as cell array
%        contents{get(hObject,'Value')} returns selected item from popupmenu13
if(get(hObject,'Value')-1)
    activate_structural(handles);
else
    deactivate_structural(handles);
end

% --- Executes during object creation, after setting all properties.
function edit53_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit53 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit53_Callback(hObject, eventdata, handles)
% hObject    handle to edit53 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit53 as text
%        str2double(get(hObject,'String')) returns contents of edit53 as a double


% --- Executes during object creation, after setting all properties.
function edit54_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit54 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit54_Callback(hObject, eventdata, handles)
% hObject    handle to edit54 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit54 as text
%        str2double(get(hObject,'String')) returns contents of edit54 as a double


% --- Executes during object creation, after setting all properties.
function edit55_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit55 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit55_Callback(hObject, eventdata, handles)
% hObject    handle to edit55 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit55 as text
%        str2double(get(hObject,'String')) returns contents of edit55 as a double


% --- Executes during object creation, after setting all properties.
function edit56_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit56 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit56_Callback(hObject, eventdata, handles)
% hObject    handle to edit56 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit56 as text
%        str2double(get(hObject,'String')) returns contents of edit56 as a double
if(str2double(get(hObject,'String'))>1500)
    set(hObject,'String',1500);
end
if(str2double(get(hObject,'String'))<300)
    set(hObject,'String',300);
end

% --- Executes during object creation, after setting all properties.
function edit58_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit58 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit58_Callback(hObject, eventdata, handles)
% hObject    handle to edit58 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit58 as text
%        str2double(get(hObject,'String')) returns contents of edit58 as a double
if(str2double(get(hObject,'String'))>1500)
    set(hObject,'String',1500);
end
if(str2double(get(hObject,'String'))<300)
    set(hObject,'String',300);
end


% --- Executes during object creation, after setting all properties.
function edit59_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit59 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit59_Callback(hObject, eventdata, handles)
% hObject    handle to edit59 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit59 as text
%        str2double(get(hObject,'String')) returns contents of edit59 as a double


% --- Executes during object creation, after setting all properties.
function edit60_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit60 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit60_Callback(hObject, eventdata, handles)
% hObject    handle to edit60 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit60 as text
%        str2double(get(hObject,'String')) returns contents of edit60 as a double


