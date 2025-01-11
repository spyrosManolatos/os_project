give_answer(){
    if [ "$1" == "Y" ]; then
        echo "Give me the values using the format you should"
        read values
         if [[ "$values" =~ ^[0-9],[^0-9,]+,[0-9]+,[^0-9,]+,(Passenger|Crew),(Yes|yes|No|no)$ ]]; then
            echo "Values are correct"
            echo "$values" >> "$working_file"
            echo "Wanna insert new line?(Y/n)"
            read answer
            give_answer $answer
        else
            echo "Values are not correct"
            exit 1
        fi
    fi
}

insert_data(){
    echo "Give me the file path (please the path of the csv file)"
    read file_dir
    echo "working directory:$(pwd)"
    if [[ $file_dir != *.csv ]]; then
        echo "The provided file is not a .csv file or you choosed to insert values manually"
        echo "You should insert values using keyboard?(Y/n)"
        read answer
        if [ "$answer" == "n" ]; then
            exit 1
        fi
        working_file="$(pwd)/passengers.csv"
        > "$working_file"
        echo "code,fullname,age,country,status,rescued" >> "$working_file"
        give_answer $answer
        exit 1
    else
        echo "The provided file is a .csv file"
        if [ -f $file_dir ]; then
            echo "The file exists and your data is in our safe hands"
            if [ ! -s "$file_dir" ]; then
            echo "The file is empty or does not have the valid format"
            exit 1
            fi
            awk -F'[;,]' 'NR>1 {
                if (NF == 0 || $0 ~ /^[[:space:]]*,,,,,[[:space:]]*$/) next
                if (!($1 ~ /^[0-9]+$/)) {
                    print "wrong format"
                    exit 1
                }
                if (!($2 ~ /^[^,]+$/)) {
                    print "wrong format"
                    exit 1
                }
                if (!($3 ~ /^[0-9]+$/)) {
                    print "wrong format"
                    exit 1
                }
                if (!($4 ~ /^[^,]+$/)) {
                    print "wrong format"
                    exit 1
                }
                if (!($5 ~ /^(Passenger|Crew)$/)) {
                    print "wrong format"
                    exit 1
                }
                if (!($6 ~ /^[[:space:]]*(Yes|yes|No|no)[[:space:]]*$/)) {
                    print "wrong format"
                    exit 1
                }
            }' "$file_dir" || { exit 1; }
            working_file=$file_dir
            
        else
            echo "However, The file does not exist"
            exit 1
        fi
        working_file=$file_dir
    fi
}
search_passenger(){
    if [ $# -gt 0 ]; then
        echo "Not proceed: that function will execute if the execution is without arguments"
        return
    fi
    echo "Enter the name or surname to search"
    read name
    # the name should be between commas
    if [[ "$name" =~ ^[^0-9]+$ ]]; then
        echo "Search term is correct"
        result=$(sed -n '2,$p' "$working_file" | awk -F, -v name="$name" '$2 ~ "(^|[ ,])" name "([ ,]|$)" {print}')
       
        if [ -z "$result" ]; then
            echo "No passenger found with the name or surname: $name"
        else
            echo "$result"
        fi

    else
        echo "Search term is not correct"
        exit 1
    fi
}
# Function to update a record by code
update_record_by_code() {
    code="$1"
    new_record="$2"
    awk -F, -v code="$code" -v new_record="$new_record" '
    BEGIN {OFS=FS}
    $1 == code {
        print "Original line: " $0 > "/dev/stderr"
        $0 = new_record
    }
    {print}
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "New record: $new_record"
}
# Function to update full name by code and display the original line
update_full_name_by_code() {
    code="$1"
    new_full_name="$2"
    awk -F, -v code="$code" -v full_name="$new_full_name" '
    BEGIN {OFS=FS}
    $1 == code {
        print "Original line: " $0 > "/dev/stderr"
        $2 = full_name
    }
    {print} 
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
}


# Function to take user input and determine the update method for records
# Function to update only the first name or surname
update_first_name_or_surname() {
    search_term="$1"
    new_value="$2"
    awk -F, -v search_term="$search_term" -v new_value="$new_value" '
    BEGIN {OFS=FS}
    $2 ~ search_term {
        print "Original line: " $0 > "/dev/stderr"
        split($2, names, " ")
        if (names[1] == search_term) {
            $2 = new_value
        } else if (names[2] == search_term) {
            $2 = new_value
        }
        
    }
    {print}
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "First name or surname updated"
}
update_record_by_name() {
    search_term="$1"
    new_record="$2"
    awk -F, -v search_term="$search_term" -v new_record="$new_record" '
    BEGIN {OFS=FS}
    $2 ~ search_term && !found {
        print "Original line: " $0 > "/dev/stderr"
        $0 = new_record
        found = 1
    }
    {print} 
    ' "$working_file" > temp.csv && mv temp.csv "$working_file"
    echo "Record updated"
}
update_passenger() {
    local search_term="$1";
    shift
    local new_value="$*";
    if [[ "$search_term" =~ ^[0-9]+$ ]] && [[ "$new_value" =~ ^[0-9]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+$ ]]; then
        update_record_by_code "$search_term" "$new_value"
    elif [[ "$search_term" =~ ^[0-9]+$ ]]; then
        update_full_name_by_code "$search_term" "$new_value"
    elif [[ "$search_term" =~ ^[^,]+$ ]] && [[ "$new_value" =~ ^[0-9]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+$ ]]; then
        if [[ "$search_term" =~ ^[0-9]+$ ]]; then
            echo "search term or new value should not be a number";
            exit;
        fi
        update_record_by_name "$search_term" "$new_value"
    elif [[ "$search_term" =~ ^[^,]+$ ]]; then
        if [[ "$search_term" =~ ^[0-9]+$ || "$new_value" =~ ^[0-9]+ ]]; then
            echo "search term or new value should not be a number";
            exit;
        fi
        update_first_name_or_surname "$search_term" "$new_value"
    else
        echo "Search term is not correct"
        exit 1
    fi
}
display_file(){
    less $1
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
        
        print "\n=== Age Group 19-35 ===\n" 
        for(i=0; i<count_19_35; i++) print group2[i]
        
        print "\n=== Age Group 36-50 ===\n" 
        for(i=0; i<count_36_50; i++) print group3[i]
        
        print "\n=== Age Group >=51 ===\n" 
        for(i=0; i<count_51; i++) print group4[i]

        print "\n=== Summary ==="
        print "Total passengers analyzed:", count_0_18 + count_19_35 + count_36_50 + count_51
        print "Total passengers in group 0-18:", count_0_18
        print "Total passengers in group 19-35:", count_19_35
        print "Total passengers in group 36-50:", count_36_50
        print "Total passengers in group >=51:", count_51


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
    awk -F, '$6 ~ /^[[:space:]]*(Yes|yes)[[:space:]]*$/' "$working_file" > rescued.txt
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
    echo "Generating reports..."
    print_lines_for_age_group
    process_age_group_csvs
    calc_avg_age_per_passnger_catagory
    filter_rescued
}
insert_data
# change_delimiter "$working_file"
if [ "$1" == "reports" ]; then
    generate_reports
elif [ $# -gt 0 ]; then
    update_passenger "$@"
else
    echo "Want to see the file or search for a passenger?(file/search)"
    read answer
    if [ "$answer" == "file" ]; then
        display_file "$working_file"
    elif [ "$answer" == "search" ]; then
        search_passenger
    fi
fi
