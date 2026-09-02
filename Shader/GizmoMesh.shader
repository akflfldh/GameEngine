{
    "Name": "GizmoMesh",
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
                "File":"GizmoMesh.hlsl",
                "VS":"VS",
                "PS":"PS"
            },

            "RenderState": {
                "CullMode": 0,
                "FillMode": 0,
                "CCW" : false,
                "DepthCompareMode": 1,
                "BlendMode": 0,
                "DepthWriteMode": 1                
            }
        }
    }
}