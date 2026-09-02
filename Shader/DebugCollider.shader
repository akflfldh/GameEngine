{
    "Name": "DebugCollider",
    "UserResource": {
         "PassBuffer": {
            "Type": "Buffer",
            "BufferID": 1,
            "BindingType":"Pass"
        },
         "ObjectBuffer": {
            "Type": "Buffer",
            "BufferID": 2,
            "BindingType":"Object"
        }
    },
    "DefaultShader": {
        
    },
    "UserShader": {
        "MainPass": {
            "OPAQUE": true,
            "InputType": 1,
            "SKINNING": false,
            "Uses": [],
            "HLSL": 
            {
                "File":"DebugCollider.hlsl",
                "VS":"VS",
                "PS":"PS"
            },

            "RenderState": {
                "CullMode": 2,
                "FillMode": 1,
                "CCW" : false,
                "DepthCompareMode": 1,
                "BlendMode": 0,
                "DepthWriteMode": 1                
            }
        }
    }
}