# Function Name
#     debug_table_register_field_gen
# Description:
#     Generate <table_name, table_index>, <table_field_name, table_field_index>,
#              <register_name, register_index>, <register_field_name, register_field_index>
#     for all supported chip from $SDK/include/hal/chipdef/xxx/*_struct.h.
# Input:
# Output:
#      array for name to index conversion C code in diag_table_reg.c
# Return:
#     Success or falid reason
##
##
# Full filename for reg/table struct and filename for generated code
set orig_dir [pwd]
#puts ${orig_dir}
set dest_filename ${orig_dir}
append dest_filename "/diag_table_reg.c"
#puts ${dest_filename}
cd ../../../../..
set src_path [pwd]
puts ${src_path}

set dest_file [open ${dest_filename} w]
puts ${dest_file} "typedef struct diag_table_reg_name_value_pair_s"
puts ${dest_file} "\{"
puts ${dest_file} "    char name\[64\];"
puts ${dest_file} "    int32 value;"
puts ${dest_file} "\} diag_table_reg_name_value_pair_t;"
puts ${dest_file} ""
puts ${dest_file} "typedef enum diag_table_reg_field_name_type_s"
puts ${dest_file} "\{"
puts ${dest_file} "    DIAG_TABLE_NAME,"
puts ${dest_file} "    DIAG_TABLE_FIELD_NAME,"
puts ${dest_file} "    DIAG_REG_NAME,"
puts ${dest_file} "    DIAG_REG_FIELD_NAME"
puts ${dest_file} "} diag_table_reg_field_name_type_t;"
puts ${dest_file} ""

array set sdk_chip_id2rtl [ list {longan} {RTL9300} {maple} {RTL8380} {cypress} {RTL8390} {mango} {RTL9310} ]

#foreach chip_name {longan maple cypress mango} {
foreach chip_name {longan mango} {
    #puts ${chip_name}
    set src_reg_filename ${src_path}
    set src_table_filename ${src_path}
    #puts ${src_table_filename}
    append src_reg_filename "/include/hal/chipdef/${chip_name}/rtk_${chip_name}_reg_struct.h"
    append src_table_filename "/include/hal/chipdef/${chip_name}/rtk_${chip_name}_table_struct.h"
    #puts ${src_reg_filename}
    #puts ${src_table_filename}

    set src_file [open ${src_reg_filename} r]
    set prefix "    "
    append prefix [string toupper ${chip_name}]
    append prefix "_"
    #process string line by line
    while {[gets ${src_file} line] >= 0} {
        if {[expr [string equal ${line} "typedef enum rtk_${chip_name}_reg_list_e"] == 1]} {
            set index 0
            #puts ${line}
            set cfg_sdk_flag $sdk_chip_id2rtl(${chip_name})
            puts ${dest_file} "#ifdef CONFIG_SDK_${cfg_sdk_flag}"
            puts ${dest_file} "static diag_table_reg_name_value_pair_t ${chip_name}_diag_reg_array\[\] ="
            puts ${dest_file} "\{"
        } elseif {[expr [string equal ${line} "\} rtk_${chip_name}_reg_list_t;"] == 1]} {
            puts ${dest_file} "    \{\"\",    ${index}\}"
            puts ${dest_file} "\};"
        } elseif {[expr [string equal ${line} "typedef enum rtk_${chip_name}_regField_list_e"] == 1]} {
            set index 0
            puts ${dest_file} ""
            puts ${dest_file} "static diag_table_reg_name_value_pair_t ${chip_name}_diag_reg_field_array\[\] ="
            puts ${dest_file} "\{"
        } elseif {[expr [string equal ${line} "\} rtk_${chip_name}_regField_list_t;"] == 1]} {
            puts ${dest_file} "    \{\"\",    ${index}\}"
            puts ${dest_file} "\};"
            puts ${dest_file} ""
        } else {
            if {[regsub ${prefix} ${line} "" name_after_trim_left]} {
                set pos [string first r ${name_after_trim_left}]
                if {[expr ${pos} == -1]} {
                    set pos [string first f ${name_after_trim_left}]
                    if {[expr ${pos} == -1]} {
                        continue
                    }
                }

                set pos [expr ${pos} - 1]
                #puts "${name_after_trim_left} ${pos}"
                set name_after_trim_right [string range ${name_after_trim_left} 0 $pos]
                #puts "${name_after_trim_right} ${pos}"
                set name_after_trim_right_lower [string tolower ${name_after_trim_right}]
                puts ${dest_file} "    \{\"${name_after_trim_right_lower}\",    ${index}\},"
                set index [expr ${index} + 1]
            }
        }
    }

    set src_file [open ${src_table_filename} r]
    # process string line by line
    while {[gets ${src_file} line] >= 0} {
        if {[expr [string equal ${line} "typedef enum rtk_${chip_name}_table_list_e"] == 1]} {
            set index 0
            #puts ${line}
            puts ${dest_file} "static diag_table_reg_name_value_pair_t ${chip_name}_diag_table_array\[\] ="
            puts ${dest_file} "\{"
        } elseif {[expr [string equal ${line} "\} rtk_${chip_name}_table_list_t;"] == 1]} {
            puts ${dest_file} "    \{\"\",    $index\}"
            puts ${dest_file} "\};"
            puts ${dest_file} ""
            puts ${dest_file} "static diag_table_reg_name_value_pair_t ${chip_name}_diag_table_field_array\[\] ="
            puts ${dest_file} "\{"
        } elseif {[regexp "typedef enum rtk_${chip_name}_" ${line}]} {
            puts ${dest_file} ""
            set index 0
        } else {
            if {[regsub ${prefix} ${line} "" name_after_trim_left]} {
                set pos [string first t ${name_after_trim_left}]
                if {[expr ${pos} == -1]} {
                    set pos [string first f ${name_after_trim_left}]
                    if {[expr ${pos} == -1]} {
                        continue
                    }
                }

                set pos [expr ${pos} - 1]
                #puts "${name_after_trim_left} ${pos}"
                set name_after_trim_right [string range ${name_after_trim_left} 0 $pos]
                #puts "${name_after_trim_right} ${pos}"
                set name_after_trim_right_lower [string tolower ${name_after_trim_right}]
                puts ${dest_file} "    \{\"${name_after_trim_right_lower}\",    ${index}\},"
                set index [expr ${index} + 1]
            }
        }
    }
    puts ${dest_file} ""
    puts ${dest_file} "    \{\"\",    0\}"
    puts ${dest_file} "\};"
    puts ${dest_file} "#endif"
    puts ${dest_file} ""
    puts ${dest_file} ""
}

cd $orig_dir
exec mv -f ./diag_table_reg.c ../../../../../include/dal/longan/

