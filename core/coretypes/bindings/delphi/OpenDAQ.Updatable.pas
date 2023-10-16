//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
//
//     RTGen (DelphiGenerator v1.10.2) on 12.08.2019 14:50:07.
// </auto-generated>
//------------------------------------------------------------------------------
unit OpenDAQ.Updatable;

interface
uses
  OpenDAQ.CoreTypes,
  OpenDAQ.ObjectPtr,
  OpenDAQ.Serializer,
  OpenDAQ.ProxyValue;

type
  {$MINENUMSIZE 4}

  IUpdatablePtr<T : IUpdatable> = interface;
  IUpdatablePtr = IUpdatablePtr<IUpdatable>;

  IUpdatablePtr<T : IUpdatable> = interface(IObjectPtr<T>)
  ['{5754725c-7996-50cd-9098-8d94e9012dba}']
    procedure Update(Mode: TConfigurationMode; Update: ISerializedObject); overload;
    procedure Update(Mode: TConfigurationMode; Update: ISerializedObjectPtr); overload;
  end;

  TUpdatablePtr<T : IUpdatable> = class(TObjectPtr<T>, IUpdatablePtr<T>, IUpdatable)
  public
    constructor Create(Obj: IBaseObject); overload; override;
    constructor Create(Obj: T); overload;

    procedure Update(Mode: TConfigurationMode; Update: ISerializedObject); overload;
    procedure Update(Mode: TConfigurationMode; Update: ISerializedObjectPtr); overload;
  private
    function IUpdatable.Update = Interface_Update;

    function Interface_Update(Mode: TConfigurationMode; Update: ISerializedObject): ErrCode; stdcall;
  end;

  TUpdatable = TUpdatablePtr<IUpdatable>;
  TUpdatablePtr = TUpdatablePtr<IUpdatable>;


implementation
uses
  OpenDAQ.CoreTypes.Errors,
  OpenDAQ.Exceptions,
  OpenDAQ.SmartPtrRegistry;


constructor TUpdatablePtr<T>.Create(Obj: T);
begin
  inherited Create(Obj);
end;

constructor TUpdatablePtr<T>.Create(Obj: IBaseObject);
begin
  inherited Create(Obj);
end;

procedure TUpdatablePtr<T>.Update(Mode: TConfigurationMode; Update: ISerializedObject);
var
  Err: ErrCode;
begin
  if not Assigned(FObject) then
    raise ERTInvalidParameterException.Create('Interface object is nil.');

  Err := FObject.Update(Mode, Update);
  CheckRtErrorInfo(Err);
end;

procedure TUpdatablePtr<T>.Update(Mode: TConfigurationMode; Update: ISerializedObjectPtr);
var
  Err: ErrCode;
  UpdateIntf: ISerializedObject;
begin
  if not Assigned(FObject) then
    raise ERTInvalidParameterException.Create('Interface object is nil.');

  if Assigned(Update) then
    UpdateIntf := Update.GetInterface()
  else
    UpdateIntf := nil;

  Err := FObject.Update(Mode, UpdateIntf);
  CheckRtErrorInfo(Err);
end;

function TUpdatablePtr<T>.Interface_Update(Mode: TConfigurationMode; Update: ISerializedObject): ErrCode; stdcall;
begin
  Result := FObject.Update(Mode, Update);
end;


initialization
  TSmartPtrRegistry.RegisterPtr(IUpdatable, IUpdatablePtr, TUpdatablePtr);

finalization
  TSmartPtrRegistry.UnregisterPtr(IUpdatable);

end.