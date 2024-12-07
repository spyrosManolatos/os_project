# Function to fetch the CSV file
fetch_csv_file() {
    echo "Give me the file path (please the path of the csv file)"
    read file_dir
    echo "Working directory: $(pwd)"
    if [[ "$file_dir" != *.csv ]]; then
        echo "The provided file is not a .csv file or you chose to insert values manually"
        echo "You should insert values using keyboard? (Y/n)"
        read answer
        if [[ "$answer" == "n" ]]; then
            exit 1
        fi
        working_file="$(pwd)/passengers.csv"
    else
        echo "The provided file is a .csv file"
        working_file=$file_dir
        if [[ -f "$working_file" ]]; then
            echo "The file exists and your data is in our safe hands"
        else
            echo "However, the file does not exist"
            exit 1
        fi
    fi
}
# Function to change delimiter from semicolon to comma
change_delimiter() {
    sed 's/;/,/g' "$working_file" > temp.csv && mv temp.csv "$working_file"
}

print_lines_for_age_group() {
    {
    awk -F, '
    BEGIN {
        count_0_18 = 0
        count_19_35 = 0
        count_36_50 = 0
        count_51 = 0
    }
    NR==1 {
        print $0
    }
    NR>1 {
        if($3 >= 0 && $3 <= 18) {
            group1[NR] = $0
            count_0_18++
        }
        if($3 > 18 && $3 <= 35) {
            group2[NR] = $0
            count_19_35++
        }
        if($3 >= 36 && $3 <=50){
            group3[NR] = $0
            count_36_50++      
        }
        if($3 >=51){
            group4[NR] = $0
            count_51++
        }
    }
    END {
        print "Ages 0-18:", count_0_18
        print "Ages 19-35:", count_19_35
        print "Ages 36-50:", count_36_50
        print "Age >51:", count_51
        print "\n=== Age Group 0-18 ===\n" 
        for(i=0; i<count_0_18; i++) print group1[i]
        print "\nTotal in group 0-18:", count_0_18
        
        print "\n=== Age Group 19-35 ===\n" 
        for(i=0; i<count_19_35; i++) print group2[i]
        print "\nTotal in group 19-35:", count_19_35
        
        print "\n=== Age Group 36-50 ===\n" 
        for(i=0; i<count_36_50; i++) print group3[i]
        print "\nTotal in group 36-50:", count_36_50
        
        print "\n=== Age Group >=51 ===\n" 
        for(i=0; i<count_51; i++) print group4[i]
        print "\nTotal in group >=51:", count_51

        print "\n=== Summary ==="
        print "Total passengers analyzed:", count_0_18 + count_19_35 + count_36_50 + count_51


    }
    ' "$working_file"
    } > ages.txt
    sed -i '/^$/N;/^\n$/D' ages.txt

}

process_age_group_csvs(){
    {
        awk -F, '
        BEGIN {
            percentage_for_0_18 = 0
            rescued_0_18 = 0
            count_0_18 = 0
            rescued_19_35 = 0
            rescued_36_50 = 0
            rescued_51 = 0
        }
        NR>1 {
            if ($3 >= 0 && $3 <= 18) {
                count_0_18++
                if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                    rescued_0_18++
                }
            }
            else if ($3 > 18 && $3 <= 35) {
                count_19_35++
                if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                    rescued_19_35++
                }
            }
            else if ($3 >= 36 && $3 <= 50) {
                count_36_50++
                if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                    rescued_36_50++
                }
            }
            else if ($3 >= 51) {
                count_51++
                if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                    rescued_51++
                }
            }
            
        }
        END {
            print "\nAge Group 0-18 Summary:"
            print "------------------------"
            print "Total passengers:", count_0_18
            print "Rescued passengers:", rescued_0_18
            if (count_0_18 > 0) {
                percentage_for_0_18 = (rescued_0_18/count_0_18) * 100
                printf "Rescue percentage: %.2f%%\n", percentage_for_0_18
            }
            print "\nAge Group 19-35 Summary:"
            print "------------------------"
            print "Total passengers:", count_19_35
            print "Rescued passengers:", rescued_19_35
            if (count_19_35 > 0) {
                percentage_for_19_35 = (rescued_19_35/count_19_35) * 100
                printf "Rescue percentage: %.2f%%\n", percentage_for_19_35
            }
            print "\nAge Group 36-50 Summary:"
            print "------------------------"
            print "Total passengers:", count_36_50
            print "Rescued passengers:", rescued_36_50
            if (count_36_50 > 0) {
                percentage_for_36_50 = (rescued_36_50/count_36_50) * 100
                printf "Rescue percentage: %.2f%%\n", percentage_for_36_50
            }
            print "\nAge Group >=51 Summary:"
            print "------------------------"
            print "Total passengers:", count_51
            print "Rescued passengers:", rescued_51
            if (count_51 > 0) {
                percentage_for_51 = (rescued_51/count_51) * 100
                printf "Rescue percentage: %.2f%%\n", percentage_for_51
            }
        }' "$working_file"
    } > percentages.txt
}
filter_rescued(){
    grep -i "yes" "$working_file" > rescued.txt
}
calc_avg_age_per_passnger_catagory(){
    {
        awk -F, '
        BEGIN {
            total_age = 0
            total_passengers = 0
            total_children = 0
            total_adults_for_19_35 = 0
            total_adults_for_35_50 = 0
            total_seniors = 0
            total_age_for_children = 0
            total_age_for_adults_for_19_35 = 0
            total_age_for_adults_for_35_50 = 0
            total_age_for_seniors = 0
        }
        NR>1 {
            total_age += $3
            total_passengers++
            
            if($5 ~ /^[[:space:]]*Passenger[[:space:]]*$/) {
                total_rescued++
                if ($3 >= 0 && $3 <= 18) {
                    total_children++
                    total_age_for_children += $3
                }
                if ($3 > 19 && $3 <= 35) {
                    total_adults_for_19_35++
                    total_age_for_adults_for_19_35 += $3
                }
                else if ($3 > 35 && $3 <= 50) {
                    total_adults_for_35_50++
                    total_age_for_adults_for_35_50 += $3
                }
                else if ($3 > 50) {
                    total_seniors++
                    total_age_for_seniors += $3
                }
            }
            
        }
        END {
            print "Average age of all passengers:", total_age/total_passengers
            print "Average age of children (0-18):", total_age_for_children/total_children
            print "Average age of adults (19-35):", total_age_for_adults_for_19_35/total_adults_for_19_35
            print "Average age of adults (36-50):", total_age_for_adults_for_35_50/total_adults_for_35_50
            print "Average age of seniors (51+):", total_age_for_seniors/total_seniors
        }' "$working_file"
    } > avg.txt
}
generate_reports(){
    print_lines_for_age_group
    process_age_group_csvs
    calc_avg_age_per_passnger_catagory
    filter_rescued
}
fetch_csv_file
change_delimiter
generate_reports
