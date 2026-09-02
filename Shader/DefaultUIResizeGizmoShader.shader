{
    "Name": "DefaultUIResizeGizmo",
    "UserResource": {
        "PassBuffer":{
            "Type":"Buffer",
            "BufferID":1,
            "BindingType": "Pass"
        }
    },
    "DefaultShader": {
        
    },
    "UserShader": {
        "MainPass": {
            "OPAQUE": true,
            "InputType": 0,
            "SKINNING": false,
            "Uses": ["PassBuffer"],
            "HLSL": 
            {
                "File":"DefaultUIResizeGizmo.hlsl",
                "VS":"VSMain",
                "PS":"PSMain"
            },

            "RenderState": {
                "CullMode": 0,
                "FillMode": 0,
                "DepthCompareMode": 1,
                "BlendMode": 0,
                "DepthWriteMode": 0                
            }
        }
    }
}