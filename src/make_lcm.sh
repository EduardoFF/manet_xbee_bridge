lcm-gen -x lcm_msgs/pose_t.lcm
lcm-gen --python --ppath python/ lcm_msgs/pose_t.lcm

lcm-gen --python --package-prefix rnp --ppath python/ lcm_msgs/route2_tree_t.lcm lcm_msgs/route2_table_t.lcm lcm_msgs/route2_entry_t.lcm
lcm-gen -x lcm_msgs/route2_tree_t.lcm lcm_msgs/route2_table_t.lcm lcm_msgs/route2_entry_t.lcm

lcm-gen  -x lcm_msgs/plan2_tree_t.lcm lcm_msgs/plan2_table_t.lcm lcm_msgs/plan2_entry_t.lcm
lcm-gen --python --package-prefix plan --ppath python/ lcm_msgs/plan2_tree_t.lcm lcm_msgs/plan2_table_t.lcm lcm_msgs/plan2_entry_t.lcm
