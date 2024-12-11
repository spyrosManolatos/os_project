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
        print "=== Age Group For Passengers ==="
    }
    NR==1 {
        print $0
    }
    NR>1 {
        if($3 >= 0 && $3 <= 18) {
            if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                group1[count_0_18++] = $0
            }
            
        }
        if($3 > 18 && $3 <= 35) {
            if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                group2[count_19_35++] = $0
            }
            
        }
        if($3 >= 36 && $3 <=50){
            if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                group3[count_36_50++] = $0
            }   
        }
        if($3 >=51){
            if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                group4[count_51++] = $0
            }
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
            percentage_for_0_18_pass = 0
            percentage_for_19_35_pass = 0
            percentage_for_36_50_pass = 0
            percentage_for_51_pass = 0

            percentage_for_0_18_crew = 0
            percentage_for_19_35_crew = 0
            percentage_for_36_50_crew = 0
            percentage_for_51_crew = 0
            
            count_0_18_for_passengers = 0
            count_19_35_for_passengers = 0
            count_36_50_for_passengers = 0
            count_51_for_passengers = 0
            count_0_18_for_crew = 0
            count_19_35_for_crew = 0
            count_36_50_for_crew = 0
            count_51_for_crew = 0

            rescued_0_18_for_passengers = 0
            rescued_19_35_for_passengers = 0
            rescued_36_50_for_passengers = 0
            rescued_51_for_passengers = 0

            rescued_0_18_for_crew = 0
            rescued_19_35_for_crew = 0
            rescued_36_50_for_crew = 0
            rescued_51_for_crew = 0
        }
        NR>1 {
            if ($3 >= 0 && $3 <= 18) {
                
                if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                    count_0_18_for_passengers++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_0_18_for_passengers++
                    }
                }
                else{
                    count_0_18_for_crew++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_0_18_for_crew++
                    }
                }
            }
            else if ($3 > 18 && $3 <= 35) {
                if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                    count_19_35_for_passengers++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_19_35_for_passengers++
                    }
                }
                else{
                    count_19_35_for_crew++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_19_35_for_crew++
                    }
                }
            }
            else if ($3 >= 36 && $3 <= 50) {
                if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                    count_36_50_for_passengers++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_36_50_for_passengers++
                    }
                }
                else{
                    count_36_50_for_crew++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_36_50_for_crew++
                    }
                }
            }
            else if ($3 >= 51) {
                if(toupper($5) ~  /^[[:space:]]*PASSENGER[[:space:]]*$/){
                    count_51_for_passengers++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_51_for_passengers++
                    }
                }
                else{
                    count_51_for_crew++
                    if (toupper($6) ~ /^[[:space:]]*YES[[:space:]]*$/) {
                        rescued_51_for_crew++
                    }
                }
            }
            
        }
        END {
            percentage_for_0_18_crew = (rescued_0_18_for_crew / count_0_18_for_crew) * 100
            percentage_for_19_35_crew = (rescued_19_35_for_crew / count_19_35_for_crew) * 100
            percentage_for_36_50_crew = (rescued_36_50_for_crew / count_36_50_for_crew) * 100
            percentage_for_51_crew = (rescued_51_for_crew / count_51_for_crew) * 100
            percentage_for_0_18_pass = (rescued_0_18_for_passengers / count_0_18_for_passengers) * 100
            percentage_for_19_35_pass = (rescued_19_35_for_passengers / count_19_35_for_passengers) * 100
            percentage_for_36_50_pass = (rescued_36_50_for_passengers / count_36_50_for_passengers) * 100
            percentage_for_51_pass = (rescued_51_for_passengers / count_51_for_passengers) * 100
            print "Rescued Report"
            print "Age Group 0-18 for passengers:", rescued_0_18_for_passengers, "out of", count_0_18_for_passengers, "(", percentage_for_0_18_pass, "%)"
            print "Age Group 19-35 for passengers:", rescued_19_35_for_passengers, "out of", count_19_35_for_passengers, "(", percentage_for_19_35_pass, "%)"
            print "Age Group 36-50 for passengers:", rescued_36_50_for_passengers, "out of", count_36_50_for_passengers, "(", percentage_for_36_50_pass, "%)"
            print "Age Group 51+ for passengers:", rescued_51_for_passengers, "out of", count_51_for_passengers, "(", percentage_for_51_pass, "%)"
            print "Age Group 0-18 for crew:", rescued_0_18_for_crew, "out of", count_0_18_for_crew, "(", percentage_for_0_18_crew, "%)"
            print "Age Group 19-35 for crew:", rescued_19_35_for_crew, "out of", count_19_35_for_crew, "(", percentage_for_19_35_crew, "%)"
            print "Age Group 36-50 for crew:", rescued_36_50_for_crew, "out of", count_36_50_for_crew, "(", percentage_for_36_50_crew, "%)"
            print "Age Group 51+ for crew:", rescued_51_for_crew, "out of", count_51_for_crew, "(", percentage_for_51_crew, "%)"
            print "Total rescued passengers:", rescued_0_18_for_passengers + rescued_19_35_for_passengers + rescued_36_50_for_passengers + rescued_51_for_passengers
            print "Total rescued crew:", rescued_0_18_for_crew + rescued_19_35_for_crew + rescued_36_50_for_crew + rescued_51_for_crew
            print "Total rescued:", rescued_0_18_for_passengers + rescued_19_35_for_passengers + rescued_36_50_for_passengers + rescued_51_for_passengers + rescued_0_18_for_crew + rescued_19_35_for_crew + rescued_36_50_for_crew + rescued_51_for_crew

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
            total_children_crew = 0
            total_adults_for_19_35_crew = 0
            total_adults_for_35_50_crew = 0
            total_seniors_crew = 0
            total_children_passengers = 0
            total_adults_for_19_35_passengers = 0
            total_adults_for_35_50_passengers = 0
            total_seniors_passengers = 0
            total_seniors = 0
            total_age_for_children_pass = 0
            total_age_for_adults_for_19_35_pass = 0
            total_age_for_adults_for_35_50_pass = 0
            total_age_for_seniors_pass = 0
            total_age_for_children_crew = 0
            total_age_for_adults_for_19_35_crew = 0
            total_age_for_adults_for_35_50_crew = 0
            total_age_for_seniors_crew = 0
        }
        NR>1 {
            total_age += $3
            total_passengers++
            
            if($5 ~ /^[[:space:]]*Passenger[[:space:]]*$/) {
                total_rescued++
                if ($3 >= 0 && $3 <= 18) {
                    total_children_passengers++
                    total_age_for_children_pass += $3
                }
                if ($3 > 19 && $3 <= 35) {
                    total_adults_for_19_35_passengers++
                    total_age_for_adults_for_19_35_pass += $3
                }
                else if ($3 > 35 && $3 <= 50) {
                    total_adults_for_35_50_passengers ++
                    total_age_for_adults_for_35_50_pass += $3
                }
                else if ($3 > 50) {
                    total_seniors_passengers++
                    total_age_for_seniors_pass += $3
                }
            }
            else {
                total_rescued++
                if ($3 >= 0 && $3 <= 18) {
                    total_children_crew++
                    total_age_for_children_crew += $3
                }
                if ($3 > 19 && $3 <= 35) {
                    total_adults_for_19_35_crew++
                    total_age_for_adults_for_19_35_crew += $3
                }
                else if ($3 > 35 && $3 <= 50) {
                    total_adults_for_35_50_crew++
                    total_age_for_adults_for_35_50_crew += $3
                }
                else if ($3 > 50) {
                    total_seniors_crew++
                    total_age_for_seniors_crew += $3
                }
            }
            
        }
        END {
            print "Average age for passengers"
            print "Children (0-18):", total_age_for_children_pass / total_children_passengers
            print "Adults (19-35):", total_age_for_adults_for_19_35_pass / total_adults_for_19_35_passengers
            print "Adults (36-50):", total_age_for_adults_for_35_50_pass / total_adults_for_35_50_passengers
            print "Seniors (51+):", total_age_for_seniors_pass / total_seniors_passengers
            print "Average age for crew"
            print "Children (0-18):", total_age_for_children_crew / total_children_crew
            print "Adults (19-35):", total_age_for_adults_for_19_35_crew / total_adults_for_19_35_crew
            print "Adults (36-50):", total_age_for_adults_for_35_50_crew / total_adults_for_35_50_crew
            print "Seniors (51+):", total_age_for_seniors_crew / total_seniors_crew
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
