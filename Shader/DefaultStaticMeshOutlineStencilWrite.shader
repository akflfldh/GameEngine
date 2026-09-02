{
    "Name": "DefaultStaticMeshOutlineStencilWrite",
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
                "File":"DefaultStaticMeshDrawOutline.hlsl",
                "VS":"VS_Stencil"
            },

            "RenderState": {
                "CullMode": 2,
                "FillMode": 0,
                "CCW" : true,
                "DepthCompareMode": 0,
                "BlendMode": 0,
                "DepthWriteMode": 0,
                "StencilWriteMode":1,
                "StencilFrontCompareMode":4,
                "StencilFrontPassOp":3,
                "StencilFrontFailOp":1
            }
        }
    }
}