/*
 * Copyright (C) 2013, 2014 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

var hasBubbles = typeof bubbleQueueServer != "undefined";
var BubblesCategory = "bubbles";

var categorizedQueuesByPlatformAndBuildType = {};

for (var i = 0; i < buildbots.length; ++i) {
    var buildbot = buildbots[i];
    for (var id in buildbot.queuesInfo) {
        if (buildbot.queuesInfo[id].combinedQueues) {
            var info = buildbot.queuesInfo[id];
            var queue = {
                id: id,
                branch: info.branch,
                platform: info.platform.name,
                heading: info.heading,
                combinedQueues: Object.keys(info.combinedQueues).map(function(combinedQueueID) { return buildbot.queues[combinedQueueID]; }),
            };
        } else
            var queue = buildbot.queues[id];

        var platformName = queue.platform;
        var platform = categorizedQueuesByPlatformAndBuildType[platformName];
        if (!platform)
            platform = categorizedQueuesByPlatformAndBuildType[platformName] = {};
        if (!platform.builders)
            platform.builders = [];

        var categoryName;
        if (queue.builder)
            categoryName = "builders";
        else if (queue.tester)
            categoryName = queue.testCategory;
        else if (queue.performance)
            categoryName = "performance";
        else if (queue.leaks)
            categoryName = "leaks";
        else if (queue.staticAnalyzer)
            categoryName = "staticAnalyzer";
        else if ("combinedQueues" in queue)
            categoryName = "combinedQueues";
        else {
            console.assert("Unknown queue type.");
            continue;
        }

        category = platform[categoryName];
        if (!category)
            category = platform[categoryName] = [];

        category.push(queue);
    }
}

if (hasBubbles) {
    for (var id in bubbleQueueServer.queues) {
        var queue = bubbleQueueServer.queues[id];
        var platform = categorizedQueuesByPlatformAndBuildType[queue.platform];
        if (!platform)
            platform = categorizedQueuesByPlatformAndBuildType[queue.platform] = {};
        if (!platform.builders)
            platform.builders = [];

        var categoryName = BubblesCategory;

        platformQueues = platform[categoryName];
        if (!platformQueues)
            platformQueues = platform[categoryName] = [];

        platformQueues.push(queue);
    }
}

var testNames = {};
testNames[Buildbot.TestCategory.WebKit2] = "WK2 Tests";
testNames[Buildbot.TestCategory.WebKit1] = "WK1 Tests";

function sortedPlatforms()
{
    var platforms = [];

    for (var platformKey in Dashboard.Platform)
        platforms.push(Dashboard.Platform[platformKey]);

    platforms.sort(function(a, b) {
        return a.order - b.order;
    });

    return platforms;
}

function updateHiddenPlatforms()
{
    var hiddenPlatforms = settings.getObject("hiddenPlatforms");
    if (!hiddenPlatforms)
        hiddenPlatforms = [];

    var platformRows = document.querySelectorAll("tr.platform");
    for (var i = 0; i < platformRows.length; ++i)
        platformRows[i].classList.remove("hidden");

    for (var i = 0; i < hiddenPlatforms.length; ++i) {
        var platformRow = document.querySelector("tr.platform." + hiddenPlatforms[i]);
        if (platformRow)
            platformRow.classList.add("hidden");
    }

    var unhideButton = document.querySelector("div.cellButton.unhide");
    if (hiddenPlatforms.length)
        unhideButton.classList.remove("hidden");
    else
        unhideButton.classList.add("hidden");
}

function documentReady()
{
    var table = document.createElement("table");
    table.classList.add("queue-grid");

    var row = document.createElement("tr");
    row.classList.add("headers");

    var header = document.createElement("th");
    var unhideButton = document.createElement("div");
    unhideButton.addEventListener("click", function () { settings.clearHiddenPlatforms(); });
    unhideButton.textContent = "Show All Platforms";
    unhideButton.classList.add("cellButton", "unhide", "hidden");
    header.appendChild(unhideButton);
    row.appendChild(header);

    header = document.createElement("th");
    header.textContent = "Builders";
    row.appendChild(header);

    for (var testerKey in Buildbot.TestCategory) {
        var header = document.createElement("th");
        header.textContent = testNames[Buildbot.TestCategory[testerKey]];
        row.appendChild(header);
    }

    var header = document.createElement("th");
    header.textContent = "Other";
    row.appendChild(header);

    table.appendChild(row);

    var platforms = sortedPlatforms();

    for (var i in platforms) {
        var platform = platforms[i];
        var platformQueues = categorizedQueuesByPlatformAndBuildType[platform.name];
        if (!platformQueues)
            continue;

        var row = document.createElement("tr");
        row.classList.add("platform");
        row.classList.add(platform.name);

        var cell = document.createElement("td");
        cell.classList.add("logo");

        var ringImage = document.createElement("img");
        ringImage.classList.add("ring");
        ringImage.title = platform.readableName;
        cell.appendChild(ringImage);

        var logoImage = document.createElement("img");
        logoImage.classList.add("logo");
        cell.appendChild(logoImage);

        var hideButton = document.createElement("div");
        hideButton.addEventListener("click", function (platformName) { return function () { settings.toggleHiddenPlatform(platformName); }; }(platform.name) );
        hideButton.textContent = "hide";
        hideButton.classList.add("cellButton", "hide");
        cell.appendChild(hideButton);

        row.appendChild(cell);

        cell = document.createElement("td");

        var view = new BuildbotBuilderQueueView(platformQueues.builders);
        cell.appendChild(view.element);
        row.appendChild(cell);

        for (var testerKey in Buildbot.TestCategory) {
            var cell = document.createElement("td");

            var testerProperty = Buildbot.TestCategory[testerKey];
            if (platformQueues[testerProperty]) {
                var view = new BuildbotTesterQueueView(platformQueues[testerProperty]);
                cell.appendChild(view.element);
            }

            row.appendChild(cell);
        }

        var cell = document.createElement("td");
        if (platformQueues.performance) {
            var view = new BuildbotPerformanceQueueView(platformQueues.performance);
            cell.appendChild(view.element);
        }

        if (platformQueues.staticAnalyzer) {
            var view = new BuildbotStaticAnalyzerQueueView(platformQueues.staticAnalyzer);
            cell.appendChild(view.element);
        }

        if (platformQueues.leaks) {
            var view = new BuildbotLeaksQueueView(platformQueues.leaks);
            cell.appendChild(view.element);
        }

        if (platformQueues[BubblesCategory]) {
            var view = new BubbleQueueView(platformQueues[BubblesCategory]);
            cell.appendChild(view.element);
        }

        // Currently, all combined queues are in Other column.
        if (platformQueues.combinedQueues) {
            for (var i = 0; i < platformQueues.combinedQueues.length; ++i) {
                var view = new BuildbotCombinedQueueView(platformQueues.combinedQueues[i]);
                cell.appendChild(view.element);
            }
        }

        row.appendChild(cell);

        table.appendChild(row);
    }

    document.body.appendChild(table);

    if (settings.available()) {
        var settingsButton = document.createElement("div");
        settingsButton.addEventListener("click", function () { settings.toggleSettingsDisplay(); });
        settingsButton.classList.add("settings");
        document.body.appendChild(settingsButton);

        updateHiddenPlatforms();
        settings.addSettingListener("hiddenPlatforms", updateHiddenPlatforms);
    }
}

webkitTrac.startPeriodicUpdates();
if (typeof internalTrac !== "undefined")
    internalTrac.startPeriodicUpdates();

document.addEventListener("DOMContentLoaded", documentReady);
