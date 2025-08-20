{
    "Name": "BaseUI",
    "UserResource": {
        "_TexMap": {
            "Type": "Tex2D",
            "FORMAT": "R8G8B8A8_UNORM"
        },
        "_LinearSampler": {
            "Type": "Sampler",
            "ID":1
        }
    },
    "DefaultShader": {
        
    },
    "UserShader": {
        "MainPass": {
            "OPAQUE": true,
            "InputType": 0,
            "SKINNING": false,
            "Uses": ["_TexMap", "_LinearSampler"],
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
                "BlendMode": 0,
                "DepthWriteMode": 0                
            }
        }
    }
}