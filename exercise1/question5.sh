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
# Function to print entire lines for ages 1-18

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

fetch_csv_file
change_delimiter
print_lines_for_age_group
