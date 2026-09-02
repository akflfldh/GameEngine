{
    "Name": "DefaultStaticMesh",
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
        },
        "_TexMap": {
            "Type": "Tex2D",
            "FORMAT": "R8G8B8A8_UNORM",
            "BindingType":"Object"
        },
        "_LinearSampler": {
            "Type": "Sampler",
            "ID":1,
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
            "Uses": ["_TexMap", "_LinearSampler"],
            "HLSL": 
            {
                "File":"DefaultStaticMesh.hlsl",
                "VS":"VS",
                "PS":"PS"
            },

            "RenderState": {
                "CullMode": 2,
                "FillMode": 0,
                "CCW" : false,
                "DepthCompareMode": 1,
                "BlendMode": 0,
                "DepthWriteMode": 1                
            }
        }
    }
}