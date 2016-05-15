all : C_table_bobox_expanded.csv

C_table_bobox.csv : ../ctb  src_common.csv src_old_io.csv 
	cat src_common.csv src_old_io.csv > C_table_bobox.csv

C_table_cf.csv : ../ctb src_common.csv src_control_flow.csv src_new_io.csv 
	cat src_common.csv src_control_flow.csv src_new_io.csv > C_table_cf.csv

C_table_cf_expanded.csv C_table_bobox_expanded.csv : C_table_bobox.csv C_table_cf.csv
	../ctb -f expand ${CTB_ARGS}

clean :
	rm C_*csv

