#!/bin/bash

# Configuration
ITERATIONS=100
LOG_FILE="benchmark_log.txt"

# 1. CLEANUP
echo "[SETUP] Cleaning previous logs..."
rm -f $LOG_FILE
touch $LOG_FILE

# 2. RUN LOOP
echo "[START] Running $ITERATIONS iterations of RDS_SGX..."
echo "---------------------------------------------------"

for ((i=1; i<=ITERATIONS; i++))
do
    # Run the app and capture output
    OUTPUT=$(./app)
    
    # Save to file for record keeping
    echo "--- RUN #$i ---" >> $LOG_FILE
    echo "$OUTPUT" >> $LOG_FILE
    
    # Print a small progress bar
    # We grep the specific latency line just to show it's working live
    LATENCY=$(echo "$OUTPUT" | grep "Avg Latency" | awk '{print $5}')
    echo -ne "Run $i/$ITERATIONS | Last Latency: $LATENCY us\r"
done

echo -e "\n\n[DONE] All runs completed. Processing data..."

# 3. CALCULATE AVERAGES using AWK
# We grep the specific lines, extract the number (column 5), and average them.

AVG_LATENCY=$(grep "Avg Latency" $LOG_FILE | awk '{ sum += $5; n++ } END { if (n > 0) print sum / n; }')
AVG_THROUGHPUT=$(grep "Throughput" $LOG_FILE | awk '{ sum += $5; n++ } END { if (n > 0) print sum / n; }')
AVG_MEMORY=$(grep "System RAM" $LOG_FILE | awk '{ sum += $6; n++ } END { if (n > 0) print sum / n; }')

# 4. PRINT FINAL REPORT
echo "========================================================"
echo "          FINAL AGGREGATED RESULTS (N=$ITERATIONS)      "
echo "========================================================"
printf "| Metric              | Average Value             |\n"
printf "|---------------------|---------------------------|\n"
printf "| 1. Avg Latency      | %10.3f us / query     |\n" "$AVG_LATENCY"
printf "| 2. Avg Throughput   | %10.0f Ops / sec      |\n" "$AVG_THROUGHPUT"
printf "| 3. Avg Memory Used  | %10.0f KB               |\n" "$AVG_MEMORY"
echo "========================================================"
echo "Raw data saved to: $LOG_FILE"
