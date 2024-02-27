function varargout = VBUDSFCgui(varargin)
% VBUDSFCGUI M-file for VBUDSFCgui.fig
%      VBUDSFCGUI, by itself, creates a new VBUDSFCGUI or raises the existing
%      singleton*.
%
%      H = VBUDSFCGUI returns the handle to a new VBUDSFCGUI or the handle to
%      the existing singleton*.
%
%      VBUDSFCGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in VBUDSFCGUI.M with the given input arguments.
%
%      VBUDSFCGUI('Property','Value',...) creates a new VBUDSFCGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before VBUDSFCgui_OpeningFunction gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to VBUDSFCgui_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help VBUDSFCgui

% Last Modified by GUIDE v2.5 15-Nov-2005 19:18:02

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @VBUDSFCgui_OpeningFcn, ...
                   'gui_OutputFcn',  @VBUDSFCgui_OutputFcn, ...
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


% --- Executes just before VBUDSFCgui is made visible.
function VBUDSFCgui_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to VBUDSFCgui (see VARARGIN)

% Choose default command line output for VBUDSFCgui
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

% UIWAIT makes VBUDSFCgui wait for user response (see UIRESUME)
% uiwait(handles.figure1);

set(handles.checkbox1,'Value',1);
set(handles.popupmenu1,'Enable','on');
set(handles.checkbox2,'Value',0);
set(handles.popupmenu2,'Enable','off');
set(handles.checkbox3,'Value',0);
set(handles.popupmenu3,'Enable','off');
set(handles.popupmenu4,'Enable','off');
set(handles.edit1,'Enable','off');
set(handles.slider1,'Enable','off');
set(handles.radiobutton1,'Enable','off');
set(handles.radiobutton2,'Enable','off');
set(handles.radiobutton3,'Enable','off');
set(handles.radiobutton4,'Enable','off');
set(handles.radiobutton5,'Enable','off');
set(handles.radiobutton6,'Enable','off');
set(handles.radiobutton7,'Enable','off');
set(handles.radiobutton8,'Enable','off');
set(handles.radiobutton9,'Enable','off');
set(handles.radiobutton10,'Enable','off');
set(handles.radiobutton11,'Enable','off');
set(handles.radiobutton12,'Enable','off');
set(handles.radiobutton13,'Enable','off');
set(handles.radiobutton14,'Enable','off');
set(handles.radiobutton15,'Enable','off');
set(handles.checkbox4,'Value',0);
set(handles.popupmenu5,'Enable','off');
set(handles.popupmenu6,'Enable','off');
set(handles.edit2,'Enable','off');
set(handles.slider2,'Enable','off');
set(handles.radiobutton49,'Enable','off');
set(handles.radiobutton50,'Enable','off');
set(handles.radiobutton51,'Enable','off');
set(handles.radiobutton52,'Enable','off');
set(handles.radiobutton53,'Enable','off');
set(handles.radiobutton54,'Enable','off');
set(handles.radiobutton55,'Enable','off');
set(handles.radiobutton56,'Enable','off');
set(handles.radiobutton57,'Enable','off');
set(handles.radiobutton58,'Enable','off');
set(handles.radiobutton59,'Enable','off');
set(handles.radiobutton60,'Enable','off');
set(handles.radiobutton61,'Enable','off');
set(handles.radiobutton62,'Enable','off');
set(handles.radiobutton63,'Enable','off');
set(handles.radiobutton64,'Enable','off');
set(handles.radiobutton65,'Enable','off');
set(handles.radiobutton66,'Enable','off');
set(handles.radiobutton67,'Enable','off');
set(handles.radiobutton68,'Enable','off');
set(handles.checkbox5,'Value',0);
set(handles.popupmenu7,'Enable','off');
set(handles.popupmenu8,'Enable','off');
set(handles.radiobutton69,'Enable','off');
set(handles.radiobutton70,'Enable','off');
set(handles.radiobutton71,'Enable','off');
set(handles.radiobutton72,'Enable','off');
set(handles.radiobutton73,'Enable','off');
set(handles.radiobutton74,'Enable','off');
set(handles.radiobutton75,'Enable','off');
set(handles.radiobutton76,'Enable','off');
set(handles.radiobutton77,'Enable','off');
set(handles.radiobutton78,'Enable','off');
set(handles.radiobutton79,'Enable','off');
set(handles.radiobutton80,'Enable','off');
set(handles.radiobutton81,'Enable','off');
set(handles.radiobutton82,'Enable','off');
set(handles.radiobutton83,'Enable','off');
set(handles.radiobutton84,'Enable','off');
set(handles.radiobutton85,'Enable','off');
set(handles.radiobutton86,'Enable','off');
set(handles.radiobutton87,'Enable','off');
set(handles.radiobutton88,'Enable','off');
set(handles.radiobutton89,'Enable','off');
set(handles.radiobutton90,'Enable','off');
set(handles.radiobutton91,'Enable','off');
set(handles.radiobutton92,'Enable','off');
set(handles.radiobutton93,'Enable','off');
set(handles.checkbox6,'Value',0);
set(handles.popupmenu9,'Enable','off');
set(handles.popupmenu10,'Enable','off');
set(handles.radiobutton94,'Enable','off');
set(handles.radiobutton95,'Enable','off');
set(handles.radiobutton96,'Enable','off');
set(handles.radiobutton97,'Enable','off');
set(handles.radiobutton98,'Enable','off');
set(handles.radiobutton99,'Enable','off');
set(handles.radiobutton100,'Enable','off');
set(handles.radiobutton101,'Enable','off');
set(handles.radiobutton102,'Enable','off');
set(handles.radiobutton103,'Enable','off');
set(handles.radiobutton104,'Enable','off');
set(handles.radiobutton105,'Enable','off');
set(handles.radiobutton106,'Enable','off');
set(handles.radiobutton107,'Enable','off');
set(handles.radiobutton108,'Enable','off');
set(handles.radiobutton109,'Enable','off');
set(handles.radiobutton110,'Enable','off');
set(handles.radiobutton111,'Enable','off');
set(handles.radiobutton112,'Enable','off');
set(handles.radiobutton113,'Enable','off');
set(handles.radiobutton114,'Enable','off');
set(handles.radiobutton115,'Enable','off');
set(handles.radiobutton116,'Enable','off');
set(handles.radiobutton117,'Enable','off');
set(handles.radiobutton118,'Enable','off');


% --- Outputs from this function are returned to the command line.
function varargout = VBUDSFCgui_OutputFcn(hObject, eventdata, handles)
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


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


% --- Executes on button press in togglebutton2.
function togglebutton2_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton2


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


% --- Executes on button press in togglebutton3.
function togglebutton3_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton3


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


% --- Executes during object creation, after setting all properties.
function slider1_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background, change
%       'usewhitebg' to 0 to use default.  See ISPC and COMPUTER.
usewhitebg = 1;
if usewhitebg
    set(hObject,'BackgroundColor',[.9 .9 .9]);
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on slider movement.
function slider1_Callback(hObject, eventdata, handles)
% hObject    handle to slider1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider


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


% --- Executes on button press in radiobutton2.
function radiobutton2_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton2


% --- Executes on button press in radiobutton3.
function radiobutton3_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton3


% --- Executes on button press in radiobutton4.
function radiobutton4_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton4


% --- Executes on button press in radiobutton5.
function radiobutton5_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton5


% --- Executes on button press in radiobutton6.
function radiobutton6_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton6


% --- Executes on button press in radiobutton7.
function radiobutton7_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton7


% --- Executes on button press in radiobutton8.
function radiobutton8_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton8 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton8


% --- Executes on button press in radiobutton9.
function radiobutton9_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton9 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton9


% --- Executes on button press in radiobutton10.
function radiobutton10_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton10 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton10


% --- Executes on button press in radiobutton11.
function radiobutton11_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton11 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton11


% --- Executes on button press in radiobutton12.
function radiobutton12_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton12 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton12


% --- Executes on button press in radiobutton13.
function radiobutton13_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton13 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton13


% --- Executes on button press in radiobutton14.
function radiobutton14_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton14 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton14


% --- Executes on button press in radiobutton15.
function radiobutton15_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton15 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton15


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


% --- Executes on button press in togglebutton4.
function togglebutton4_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton4


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


% --- Executes during object creation, after setting all properties.
function slider2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: slider controls usually have a light gray background, change
%       'usewhitebg' to 0 to use default.  See ISPC and COMPUTER.
usewhitebg = 1;
if usewhitebg
    set(hObject,'BackgroundColor',[.9 .9 .9]);
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end


% --- Executes on slider movement.
function slider2_Callback(hObject, eventdata, handles)
% hObject    handle to slider2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'Value') returns position of slider
%        get(hObject,'Min') and get(hObject,'Max') to determine range of slider


% --- Executes during object creation, after setting all properties.
function edit2_CreateFcn(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc
    set(hObject,'BackgroundColor','white');
else
    set(hObject,'BackgroundColor',get(0,'defaultUicontrolBackgroundColor'));
end



function edit2_Callback(hObject, eventdata, handles)
% hObject    handle to edit2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of edit2 as text
%        str2double(get(hObject,'String')) returns contents of edit2 as a double


% --- Executes on button press in radiobutton49.
function radiobutton49_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton49 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton49


% --- Executes on button press in radiobutton50.
function radiobutton50_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton50 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton50


% --- Executes on button press in radiobutton51.
function radiobutton51_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton51 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton51


% --- Executes on button press in radiobutton52.
function radiobutton52_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton52 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton52


% --- Executes on button press in radiobutton53.
function radiobutton53_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton53 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton53


% --- Executes on button press in radiobutton54.
function radiobutton54_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton54 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton54


% --- Executes on button press in radiobutton55.
function radiobutton55_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton55 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton55


% --- Executes on button press in radiobutton56.
function radiobutton56_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton56 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton56


% --- Executes on button press in radiobutton57.
function radiobutton57_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton57 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton57


% --- Executes on button press in radiobutton58.
function radiobutton58_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton58 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton58


% --- Executes on button press in radiobutton59.
function radiobutton59_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton59 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton59


% --- Executes on button press in radiobutton60.
function radiobutton60_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton60 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton60


% --- Executes on button press in radiobutton61.
function radiobutton61_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton61 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton61


% --- Executes on button press in radiobutton62.
function radiobutton62_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton62 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton62


% --- Executes on button press in radiobutton63.
function radiobutton63_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton63 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton63


% --- Executes on button press in radiobutton64.
function radiobutton64_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton64 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton64


% --- Executes on button press in radiobutton65.
function radiobutton65_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton65 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton65


% --- Executes on button press in radiobutton66.
function radiobutton66_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton66 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton66


% --- Executes on button press in radiobutton67.
function radiobutton67_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton67 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton67


% --- Executes on button press in radiobutton68.
function radiobutton68_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton68 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton68


% --- Executes on button press in togglebutton5.
function togglebutton5_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton5


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


% --- Executes on button press in togglebutton6.
function togglebutton6_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton6


% --- Executes on button press in radiobutton69.
function radiobutton69_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton69 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton69


% --- Executes on button press in radiobutton70.
function radiobutton70_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton70 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton70


% --- Executes on button press in radiobutton71.
function radiobutton71_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton71 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton71


% --- Executes on button press in radiobutton72.
function radiobutton72_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton72 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton72


% --- Executes on button press in radiobutton73.
function radiobutton73_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton73 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton73


% --- Executes on button press in radiobutton74.
function radiobutton74_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton74 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton74


% --- Executes on button press in radiobutton75.
function radiobutton75_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton75 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton75


% --- Executes on button press in radiobutton76.
function radiobutton76_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton76 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton76


% --- Executes on button press in radiobutton77.
function radiobutton77_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton77 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton77


% --- Executes on button press in radiobutton78.
function radiobutton78_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton78 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton78


% --- Executes on button press in radiobutton79.
function radiobutton79_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton79 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton79


% --- Executes on button press in radiobutton80.
function radiobutton80_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton80 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton80


% --- Executes on button press in radiobutton81.
function radiobutton81_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton81 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton81


% --- Executes on button press in radiobutton82.
function radiobutton82_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton82 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton82


% --- Executes on button press in radiobutton83.
function radiobutton83_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton83 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton83


% --- Executes on button press in radiobutton84.
function radiobutton84_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton84 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton84


% --- Executes on button press in radiobutton85.
function radiobutton85_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton85 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton85


% --- Executes on button press in radiobutton86.
function radiobutton86_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton86 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton86


% --- Executes on button press in radiobutton87.
function radiobutton87_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton87 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton87


% --- Executes on button press in radiobutton88.
function radiobutton88_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton88 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton88


% --- Executes on button press in radiobutton89.
function radiobutton89_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton89 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton89


% --- Executes on button press in radiobutton90.
function radiobutton90_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton90 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton90


% --- Executes on button press in radiobutton91.
function radiobutton91_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton91 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton91


% --- Executes on button press in radiobutton92.
function radiobutton92_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton92 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton92


% --- Executes on button press in radiobutton93.
function radiobutton93_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton93 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton93


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


% --- Executes on button press in togglebutton7.
function togglebutton7_Callback(hObject, eventdata, handles)
% hObject    handle to togglebutton7 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of togglebutton7


% --- Executes on button press in radiobutton94.
function radiobutton94_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton94 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton94


% --- Executes on button press in radiobutton95.
function radiobutton95_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton95 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton95


% --- Executes on button press in radiobutton96.
function radiobutton96_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton96 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton96


% --- Executes on button press in radiobutton97.
function radiobutton97_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton97 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton97


% --- Executes on button press in radiobutton98.
function radiobutton98_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton98 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton98


% --- Executes on button press in radiobutton99.
function radiobutton99_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton99 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton99


% --- Executes on button press in radiobutton100.
function radiobutton100_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton100 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton100


% --- Executes on button press in radiobutton101.
function radiobutton101_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton101 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton101


% --- Executes on button press in radiobutton102.
function radiobutton102_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton102 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton102


% --- Executes on button press in radiobutton103.
function radiobutton103_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton103 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton103


% --- Executes on button press in radiobutton104.
function radiobutton104_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton104 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton104


% --- Executes on button press in radiobutton105.
function radiobutton105_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton105 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton105


% --- Executes on button press in radiobutton106.
function radiobutton106_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton106 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton106


% --- Executes on button press in radiobutton107.
function radiobutton107_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton107 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton107


% --- Executes on button press in radiobutton108.
function radiobutton108_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton108 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton108


% --- Executes on button press in radiobutton109.
function radiobutton109_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton109 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton109


% --- Executes on button press in radiobutton110.
function radiobutton110_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton110 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton110


% --- Executes on button press in radiobutton111.
function radiobutton111_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton111 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton111


% --- Executes on button press in radiobutton112.
function radiobutton112_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton112 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton112


% --- Executes on button press in radiobutton113.
function radiobutton113_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton113 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton113


% --- Executes on button press in radiobutton114.
function radiobutton114_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton114 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton114


% --- Executes on button press in radiobutton115.
function radiobutton115_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton115 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton115


% --- Executes on button press in radiobutton116.
function radiobutton116_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton116 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton116


% --- Executes on button press in radiobutton117.
function radiobutton117_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton117 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton117


% --- Executes on button press in radiobutton118.
function radiobutton118_Callback(hObject, eventdata, handles)
% hObject    handle to radiobutton118 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of radiobutton118


% --- Executes on button press in checkbox1.
function checkbox1_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox1
if(get(hObject,'Value'))
    set(handles.popupmenu1,'Enable','on');
else
    set(handles.popupmenu1,'Enable','off');
end

% --- Executes on button press in checkbox2.
function checkbox2_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox2
if(get(hObject,'Value'))
    set(handles.popupmenu2,'Enable','on');
else
    set(handles.popupmenu2,'Enable','off');
end


% --- Executes on button press in checkbox3.
function checkbox3_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox3
if(get(hObject,'Value'))
    set(handles.popupmenu3,'Enable','on');
    set(handles.popupmenu4,'Enable','on');
    set(handles.edit1,'Enable','on');
    set(handles.slider1,'Enable','on');
    set(handles.radiobutton1,'Enable','on');
    set(handles.radiobutton2,'Enable','on');
    set(handles.radiobutton3,'Enable','on');
    set(handles.radiobutton4,'Enable','on');
    set(handles.radiobutton5,'Enable','on');
    set(handles.radiobutton6,'Enable','on');
    set(handles.radiobutton7,'Enable','on');
    set(handles.radiobutton8,'Enable','on');
    set(handles.radiobutton9,'Enable','on');
    set(handles.radiobutton10,'Enable','on');
    set(handles.radiobutton11,'Enable','on');
    set(handles.radiobutton12,'Enable','on');
    set(handles.radiobutton13,'Enable','on');
    set(handles.radiobutton14,'Enable','on');
    set(handles.radiobutton15,'Enable','on');
else
    set(handles.popupmenu3,'Enable','off');
    set(handles.popupmenu4,'Enable','off');
    set(handles.edit1,'Enable','off');
    set(handles.slider1,'Enable','off');
    set(handles.radiobutton1,'Enable','off');
    set(handles.radiobutton2,'Enable','off');
    set(handles.radiobutton3,'Enable','off');
    set(handles.radiobutton4,'Enable','off');
    set(handles.radiobutton5,'Enable','off');
    set(handles.radiobutton6,'Enable','off');
    set(handles.radiobutton7,'Enable','off');
    set(handles.radiobutton8,'Enable','off');
    set(handles.radiobutton9,'Enable','off');
    set(handles.radiobutton10,'Enable','off');
    set(handles.radiobutton11,'Enable','off');
    set(handles.radiobutton12,'Enable','off');
    set(handles.radiobutton13,'Enable','off');
    set(handles.radiobutton14,'Enable','off');
    set(handles.radiobutton15,'Enable','off');
end


% --- Executes on button press in checkbox4.
function checkbox4_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox4 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox4
if(get(hObject,'Value'))
    set(handles.popupmenu5,'Enable','on');
    set(handles.popupmenu6,'Enable','on');
	set(handles.edit2,'Enable','on');
	set(handles.slider2,'Enable','on');
	set(handles.radiobutton49,'Enable','on');
	set(handles.radiobutton50,'Enable','on');
	set(handles.radiobutton51,'Enable','on');
	set(handles.radiobutton52,'Enable','on');
	set(handles.radiobutton53,'Enable','on');
	set(handles.radiobutton54,'Enable','on');
	set(handles.radiobutton55,'Enable','on');
	set(handles.radiobutton56,'Enable','on');
	set(handles.radiobutton57,'Enable','on');
	set(handles.radiobutton58,'Enable','on');
	set(handles.radiobutton59,'Enable','on');
	set(handles.radiobutton60,'Enable','on');
	set(handles.radiobutton61,'Enable','on');
	set(handles.radiobutton62,'Enable','on');
	set(handles.radiobutton63,'Enable','on');
	set(handles.radiobutton64,'Enable','on');
	set(handles.radiobutton65,'Enable','on');
	set(handles.radiobutton66,'Enable','on');
	set(handles.radiobutton67,'Enable','on');
	set(handles.radiobutton68,'Enable','on');    
else
    set(handles.popupmenu5,'Enable','off');
    set(handles.popupmenu6,'Enable','off');
	set(handles.edit2,'Enable','off');
	set(handles.slider2,'Enable','off');
	set(handles.radiobutton49,'Enable','off');
	set(handles.radiobutton50,'Enable','off');
	set(handles.radiobutton51,'Enable','off');
	set(handles.radiobutton52,'Enable','off');
	set(handles.radiobutton53,'Enable','off');
	set(handles.radiobutton54,'Enable','off');
	set(handles.radiobutton55,'Enable','off');
	set(handles.radiobutton56,'Enable','off');
	set(handles.radiobutton57,'Enable','off');
	set(handles.radiobutton58,'Enable','off');
	set(handles.radiobutton59,'Enable','off');
	set(handles.radiobutton60,'Enable','off');
	set(handles.radiobutton61,'Enable','off');
	set(handles.radiobutton62,'Enable','off');
	set(handles.radiobutton63,'Enable','off');
	set(handles.radiobutton64,'Enable','off');
	set(handles.radiobutton65,'Enable','off');
	set(handles.radiobutton66,'Enable','off');
	set(handles.radiobutton67,'Enable','off');
	set(handles.radiobutton68,'Enable','off');     
end

% --- Executes on button press in checkbox5.
function checkbox5_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox5 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox5
if(get(hObject,'Value'))
	set(handles.popupmenu7,'Enable','on');
	set(handles.popupmenu8,'Enable','on');
	set(handles.radiobutton69,'Enable','on');
	set(handles.radiobutton70,'Enable','on');
	set(handles.radiobutton71,'Enable','on');
	set(handles.radiobutton72,'Enable','on');
	set(handles.radiobutton73,'Enable','on');
	set(handles.radiobutton74,'Enable','on');
	set(handles.radiobutton75,'Enable','on');
	set(handles.radiobutton76,'Enable','on');
	set(handles.radiobutton77,'Enable','on');
	set(handles.radiobutton78,'Enable','on');
	set(handles.radiobutton79,'Enable','on');
	set(handles.radiobutton80,'Enable','on');
	set(handles.radiobutton81,'Enable','on');
	set(handles.radiobutton82,'Enable','on');
	set(handles.radiobutton83,'Enable','on');
	set(handles.radiobutton84,'Enable','on');
	set(handles.radiobutton85,'Enable','on');
	set(handles.radiobutton86,'Enable','on');
	set(handles.radiobutton87,'Enable','on');
	set(handles.radiobutton88,'Enable','on');
	set(handles.radiobutton89,'Enable','on');
	set(handles.radiobutton90,'Enable','on');
	set(handles.radiobutton91,'Enable','on');
	set(handles.radiobutton92,'Enable','on');
	set(handles.radiobutton93,'Enable','on');    
else
	set(handles.popupmenu7,'Enable','off');
	set(handles.popupmenu8,'Enable','off');
	set(handles.radiobutton69,'Enable','off');
	set(handles.radiobutton70,'Enable','off');
	set(handles.radiobutton71,'Enable','off');
	set(handles.radiobutton72,'Enable','off');
	set(handles.radiobutton73,'Enable','off');
	set(handles.radiobutton74,'Enable','off');
	set(handles.radiobutton75,'Enable','off');
	set(handles.radiobutton76,'Enable','off');
	set(handles.radiobutton77,'Enable','off');
	set(handles.radiobutton78,'Enable','off');
	set(handles.radiobutton79,'Enable','off');
	set(handles.radiobutton80,'Enable','off');
	set(handles.radiobutton81,'Enable','off');
	set(handles.radiobutton82,'Enable','off');
	set(handles.radiobutton83,'Enable','off');
	set(handles.radiobutton84,'Enable','off');
	set(handles.radiobutton85,'Enable','off');
	set(handles.radiobutton86,'Enable','off');
	set(handles.radiobutton87,'Enable','off');
	set(handles.radiobutton88,'Enable','off');
	set(handles.radiobutton89,'Enable','off');
	set(handles.radiobutton90,'Enable','off');
	set(handles.radiobutton91,'Enable','off');
	set(handles.radiobutton92,'Enable','off');
	set(handles.radiobutton93,'Enable','off');     
end

% --- Executes on button press in checkbox6.
function checkbox6_Callback(hObject, eventdata, handles)
% hObject    handle to checkbox6 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of checkbox6

if(get(hObject,'Value'))
	set(handles.popupmenu9,'Enable','on');
	set(handles.popupmenu10,'Enable','on');
	set(handles.radiobutton94,'Enable','on');
	set(handles.radiobutton95,'Enable','on');
	set(handles.radiobutton96,'Enable','on');
	set(handles.radiobutton97,'Enable','on');
	set(handles.radiobutton98,'Enable','on');
	set(handles.radiobutton99,'Enable','on');
	set(handles.radiobutton100,'Enable','on');
	set(handles.radiobutton101,'Enable','on');
	set(handles.radiobutton102,'Enable','on');
	set(handles.radiobutton103,'Enable','on');
	set(handles.radiobutton104,'Enable','on');
	set(handles.radiobutton105,'Enable','on');
	set(handles.radiobutton106,'Enable','on');
	set(handles.radiobutton107,'Enable','on');
	set(handles.radiobutton108,'Enable','on');
	set(handles.radiobutton109,'Enable','on');
	set(handles.radiobutton110,'Enable','on');
	set(handles.radiobutton111,'Enable','on');
	set(handles.radiobutton112,'Enable','on');
	set(handles.radiobutton113,'Enable','on');
	set(handles.radiobutton114,'Enable','on');
	set(handles.radiobutton115,'Enable','on');
	set(handles.radiobutton116,'Enable','on');
	set(handles.radiobutton117,'Enable','on');
	set(handles.radiobutton118,'Enable','on');    
else
	set(handles.popupmenu9,'Enable','off');
	set(handles.popupmenu10,'Enable','off');
	set(handles.radiobutton94,'Enable','off');
	set(handles.radiobutton95,'Enable','off');
	set(handles.radiobutton96,'Enable','off');
	set(handles.radiobutton97,'Enable','off');
	set(handles.radiobutton98,'Enable','off');
	set(handles.radiobutton99,'Enable','off');
	set(handles.radiobutton100,'Enable','off');
	set(handles.radiobutton101,'Enable','off');
	set(handles.radiobutton102,'Enable','off');
	set(handles.radiobutton103,'Enable','off');
	set(handles.radiobutton104,'Enable','off');
	set(handles.radiobutton105,'Enable','off');
	set(handles.radiobutton106,'Enable','off');
	set(handles.radiobutton107,'Enable','off');
	set(handles.radiobutton108,'Enable','off');
	set(handles.radiobutton109,'Enable','off');
	set(handles.radiobutton110,'Enable','off');
	set(handles.radiobutton111,'Enable','off');
	set(handles.radiobutton112,'Enable','off');
	set(handles.radiobutton113,'Enable','off');
	set(handles.radiobutton114,'Enable','off');
	set(handles.radiobutton115,'Enable','off');
	set(handles.radiobutton116,'Enable','off');
	set(handles.radiobutton117,'Enable','off');
	set(handles.radiobutton118,'Enable','off');     
end


% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject, eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

set(handles.pushbutton1,'String','Wait...');
