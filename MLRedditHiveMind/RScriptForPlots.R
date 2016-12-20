##########################################################
#      load required library
##########################################################

if (!require("wordcloud")) {
  install.packages("wordcloud")
  library(wordcloud)
}
if (!require("RColorBrewer")) {
  install.packages("RColorBrewer")
  library(RColorBrewer)
}

setwd("/Volumes/Transcend/MLRedditHiveMind/")

##########################################################
#      read data from the top 100 words generated
#      and plot by using wordcloud
##########################################################
fileNames <- c('politics_100','technology_100','worldnews_100')

wordsPlot <- function(fileName){
df <- read.csv(paste("data/",fileName,".csv",sep=''),header = TRUE)
df <- df[,-1]

png(paste("imgs/",fileName,".png",sep=""))
wordcloud(toupper(df$key),
          df$count,
          scale=c(5,.1),
          random.order=F,
          rot.per=.10,
          max.words=5000,
          colors=brewer.pal(8, "Dark2"),
          family="Avenir Next Condensed Bold",
          random.color=T)
dev.off()
}

for (i in 1:3){
  wordsPlot(fileNames[i])
}

##########################################################
#      read data from all data in each subreddit
#      and plot histogram
##########################################################
fileNames <- c('politics','technology','worldnews')

histPlot <- function(fileName) {
  df <- read.csv(paste("data/",fileName,".csv",sep=''),header = TRUE)
  df <- df[,-1]
  png(paste("imgs/",fileName,"_score_hist.png",sep=""))
  hist(df$score,xlim=c(-6, 10),breaks=4000)
  dev.off()
  
  countdf <- read.csv(paste("data/",fileName,"_raw.csv",sep=''),header = FALSE)
  png(paste("imgs/",fileName,"_raw_count_hist.png",sep=""))
  hist(log(df[,1]))
  dev.off()  
}

for (i in 1:3){
  histPlot(fileNames[i])
}

histForCountry <- function(fileName) {
  df <- read.csv(paste("data/",fileName,".csv",sep=''),header = TRUE)
  png(paste("imgs/",fileName,"_targetPol_hist.png",sep=""))
  hist(log(df$targetPol))
  dev.off()
}

df <- read.csv(paste("data/","americaData",".csv",sep=''),header = TRUE)
png(paste("imgs/","americaData","_score_hist.png",sep=""))
hist(df$score,xlim=c(-6,10),breaks=4000)
dev.off()



countryFileNames <- c("AppleData",
                      "ISIS_Data",
                      "ObamaData",
                      "RepublicansData",
                      "SandersData",
                      "TeslaData",
                      "WindowsData",
                      "americaData",
                      "chinaData")

for (i in 1:9){
  histForCountry(countryFileNames[i])
}