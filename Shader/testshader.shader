{
    "Name": "MyShader",
    "UserResource": {
        "PassBuffer": {
            "Type": "Buffer",
            "BufferID": 1,
            "BindingType":"Pass"
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
        "Shadow": "Default"
    },
    "UserShader": {
        "MainPass": {
            "OPAQUE": true,
            "InputType": 0,
            "Uses": ["_TexMap", "PassBuffer","_LinearSampler"],
            "HLSL": {
                "File": "BaseUI.hlsl",
                "VS":"VS",
                "PS":"PS"
            },
            "RenderState": {
                "CullMode": 2,
                "FillMode": 0,
                "DepthCompareMode": 1,
                "CCW" : true,
                "BlendMode": 0,
                "DepthWriteMode": 1
            }



        }
    }
}