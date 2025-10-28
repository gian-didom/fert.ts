{
  "targets": [
    {
      "target_name": "fert-node",
      "type": "shared_library",
      "actions": [
        {
          "action_name": "cmake_build",
          "inputs": [],
          "outputs": ["<(PRODUCT_DIR)/fert-node.node"],
          "action": [
            "cmake-js", "build", "--out", "<(PRODUCT_DIR)"
          ]
        }
      ]
    }
  ]
}
