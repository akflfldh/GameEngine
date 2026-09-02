{
    "Name": "BaseUI",
    "UserResource": {
        "PassBuffer" :{
            "Type":"Buffer",
            "BufferID":1,
            "BindingType": "Pass"
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
            "InputType": 0,
            "SKINNING": false,
            "Uses": ["_TexMap", "_LinearSampler","PassBuffer"],
            "HLSL": 
            {
                "File":"DefaultUI.hlsl",
                "VS":"VSMain",
                "PS":"PSMain"
            },

            "RenderState": {
                "CullMode": 0,
                "FillMode": 0,
                "DepthCompareMode": 1,
                "BlendMode": 1,
                "DepthWriteMode": 0,    
                "BlendSrc" : "BLEND_SRC_ALPHA",
                "BlendDest" : "BLEND_INV_SRC_ALPHA",
                "BlendOp" :  "ADD"     
            }
        }
    }
}